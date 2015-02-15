/* 
 * File:   json.cpp
 * Author: alex
 * 
 * Created on January 1, 2015, 6:18 PM
 */

#include <memory>
#include <string>
#include <cstdint>
#include <vector>
#include <utility>
#include <algorithm>

#include "jansson.h"
// declaration from
//#include "hashtable.h"
extern "C" size_t hashtable_iter_serial(void *iter);

#include "staticlib/utils.hpp"

#include "staticlib/serialization/SerializationException.hpp"
#include "staticlib/serialization/json.hpp"

#include "JanssonDeleter.hpp"

namespace staticlib {
namespace serialization {

namespace ss = staticlib::utils;
namespace sr = staticlib::reflection;

namespace { // anonymous, dump part

// forward declaration
std::unique_ptr<json_t, JanssonDeleter> dump_internal(const sr::ReflectedValue& value);

std::unique_ptr<json_t, JanssonDeleter> dump_null() {
    auto json_p = json_null();
    if (!json_p) throw SerializationException(TRACEMSG("Error initializing JSON null"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}

std::unique_ptr<json_t, JanssonDeleter> dump_object(const std::vector<sr::ReflectedField>& objectValue) {
    auto json_p = json_object();
    if (!json_p) throw SerializationException(TRACEMSG("Error initializing JSON object"));
    std::unique_ptr<json_t, JanssonDeleter> obj{json_p, JanssonDeleter()};
    for (const auto& va : objectValue) {
        auto jval = dump_internal(va.get_value());
        auto err = json_object_set(obj.get(), va.get_name().c_str(), jval.get());
        if (err) throw SerializationException(TRACEMSG(std::string(
                "Error setting field to JSON object, field name:[").append(va.get_name()).append("]")));
    }
    return obj;
}

std::unique_ptr<json_t, JanssonDeleter> dump_array(const std::vector<sr::ReflectedValue>& arrayValue) {
    auto json_p = json_array();
    if (!json_p) throw SerializationException(TRACEMSG("Error initializing JSON array"));
    std::unique_ptr<json_t, JanssonDeleter> arr{json_p, JanssonDeleter()};
    for(const auto& va : arrayValue) {
        auto jval = dump_internal(va);
        auto err = json_array_append(arr.get(), jval.get());
        if (err) throw SerializationException(TRACEMSG(
                "Error appending to JSON array, input size:[")
                .append(ss::to_string(arrayValue.size()).append("]")));                
    }
    return arr;
}

std::unique_ptr<json_t, JanssonDeleter> dump_string(const std::string& stringValue) {
    auto json_p = json_string(stringValue.c_str());
    if (!json_p) throw SerializationException(TRACEMSG(
            "Error initializing JSON with value:[").append(stringValue).append("]"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}

std::unique_ptr<json_t, JanssonDeleter> dump_integer(int64_t integerValue) {
    auto json_p = json_integer(integerValue);
    if (!json_p) throw SerializationException(TRACEMSG(
            "Error initializing JSON with value:[").append(ss::to_string(integerValue)).append("]"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}

std::unique_ptr<json_t, JanssonDeleter> dump_real(double realValue) {
    auto json_p = json_real(realValue);
    if (!json_p) throw SerializationException(TRACEMSG(
            "Error initializing JSON with value:[").append(ss::to_string(realValue)).append("]"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}

std::unique_ptr<json_t, JanssonDeleter> dump_boolean(bool booleanValue) {
    auto json_p = json_boolean(booleanValue);
    if (!json_p) throw SerializationException(TRACEMSG(
            "Error initializing JSON with value:[").append(ss::to_string(booleanValue)).append("]"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}

std::unique_ptr<json_t, JanssonDeleter> dump_internal(const sr::ReflectedValue& value) {
    switch (value.get_type()) {
    case (sr::ReflectedType::NULL_T): return dump_null();
    case (sr::ReflectedType::OBJECT): return dump_object(value.get_object());
    case (sr::ReflectedType::ARRAY): return dump_array(value.get_array());
    case (sr::ReflectedType::STRING): return dump_string(value.get_string());
    case (sr::ReflectedType::INTEGER): return dump_integer(value.get_integer());
    case (sr::ReflectedType::REAL): return dump_real(value.get_real());
    case (sr::ReflectedType::BOOLEAN): return dump_boolean(value.get_boolean());
    default: throw SerializationException(TRACEMSG(std::string(
            "Unsupported JSON type:[").append(ss::to_string(static_cast<char>(value.get_type()))).append("]")));
    }
}

std::string json_to_string(const std::unique_ptr<json_t, JanssonDeleter>& json) {
    auto json_bytes_p = json_dumps(json.get(), JSON_INDENT(4) | JSON_PRESERVE_ORDER);
    if (!json_bytes_p) throw SerializationException(TRACEMSG(std::string(
            "Error dumping JSON type: [").append(ss::to_string(json_typeof(json.get()))).append("]")));
    std::unique_ptr<char, ss::free_deleter<char>> json_bytes{json_bytes_p, ss::free_deleter<char>()};
    return std::string(json_bytes.get());
}
    
} // namespace

namespace { // anonymous, loads part

// forward declaration
sr::ReflectedValue load_internal(json_t* value);

sr::ReflectedValue load_object(json_t* value) {
    std::vector<sr::ReflectedField> obj{};
    size_t size = json_object_size(value);
    obj.reserve(size);
    
    // https://github.com/akheron/jansson/blob/23b1b7ba9a6bfce36d6e42623146c815e6b4e234/src/dump.c#L302
    std::vector<std::pair<std::string, size_t>> keys{};
    keys.reserve(size);
    
    auto iter = json_object_iter(value);
    while (iter) {
        auto serial = hashtable_iter_serial(iter);
        auto key = json_object_iter_key(iter);
        keys.emplace_back(key, serial);
        iter = json_object_iter_next(value, iter);
    }
    std::sort(keys.begin(), keys.end(), [](const std::pair<std::string, size_t>& left, 
            const std::pair<std::string, size_t>& right) {
        return left.second < right.second;
    });
    for (auto&& pair : keys) {
        auto va = json_object_get(value, pair.first.c_str());
        obj.emplace_back(std::move(pair.first), load_internal(va));
    }

    return sr::ReflectedValue(std::move(obj));
}

sr::ReflectedValue load_array(json_t* value) {
    std::vector<sr::ReflectedValue> arr{};
    arr.reserve(json_array_size(value));    
    size_t i; json_t* va;
    json_array_foreach(value, i, va) {
        arr.push_back(load_internal(va));        
    }
    return sr::ReflectedValue(std::move(arr));
}

sr::ReflectedValue load_string(json_t* value) {
    auto st = json_string_value(value);
    if(!st) throw SerializationException(TRACEMSG(std::string(
            "Error getting string value from JSON, type:[")
            .append(ss::to_string(json_typeof(value))).append("]")));
    return sr::ReflectedValue(st);
}

sr::ReflectedValue load_internal(json_t* value) {
    json_type type = json_typeof(value);
    switch (type) {
    case (JSON_NULL): return sr::ReflectedValue();
    case (JSON_OBJECT): return load_object(value);
    case (JSON_ARRAY): return load_array(value);
    case (JSON_STRING): return load_string(value);
    case (JSON_INTEGER): return sr::ReflectedValue(static_cast<int64_t>(json_integer_value(value)));
    case (JSON_REAL): return sr::ReflectedValue(json_real_value(value));
    case (JSON_TRUE): return sr::ReflectedValue(true);
    case (JSON_FALSE): return sr::ReflectedValue(false);
    default: throw SerializationException(TRACEMSG(
            "Unsupported JSON type:[").append(ss::to_string(type).append("]")));
    }
}

std::unique_ptr<json_t, JanssonDeleter> json_from_string(const std::string& str) {
    json_error_t error;
    auto flags = JSON_REJECT_DUPLICATES | JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK;
    auto json_p = json_loads(str.c_str(), flags, &error);
    if (!json_p) throw SerializationException(TRACEMSG(std::string(
            "Error parsing JSON: [").append(str).append("]")
            .append(" text: [").append(error.text).append("]")
            .append(" line: [").append(ss::to_string(error.line)).append("]")
            .append(" column: [").append(ss::to_string(error.column)).append("]")
            .append(" position: [").append(ss::to_string(error.position)).append("]")));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}

} // namespace

void init() {
    json_object_seed(0);
}

std::string dumps_json(const sr::ReflectedValue& value) {
    auto json = dump_internal(value);
    return json_to_string(json);
}

sr::ReflectedValue loads_json(const std::string& str) {
    auto json = json_from_string(str);
    return load_internal(json.get());
}

}
} // namespace

