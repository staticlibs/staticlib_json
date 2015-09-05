/*
 * Copyright 2015, alex at staticlibs.net
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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

#ifdef STATICLIB_WITH_ICU
#include <unicode/unistr.h>
#endif // STATICLIB_WITH_ICU

#include "jansson.h"
// declaration from
//#include "hashtable.h"
extern "C" size_t hashtable_iter_serial(void *iter);

#include "staticlib/serialization/SerializationException.hpp"
#include "staticlib/serialization/json.hpp"

#include "serialization_utils.hpp"
#include "JanssonDeleter.hpp"

namespace staticlib {
namespace serialization {

namespace sr = staticlib::reflection;

namespace { // anonymous, dump part

// forward declaration
std::unique_ptr<json_t, JanssonDeleter> dump_internal(const sr::ReflectedValue& value);

std::unique_ptr<json_t, JanssonDeleter> dump_null() {
    auto json_p = json_null();
    if (!json_p) throw SerializationException(SERIALIZATION_TRACEMSG("Error initializing JSON null"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}

#ifdef STATICLIB_WITH_ICU
std::unique_ptr<json_t, JanssonDeleter> dump_object(const std::vector<sr::ReflectedField>& objectValue) {
    auto json_p = json_object();
    if (!json_p) throw SerializationException(SERIALIZATION_TRACEMSG("Error initializing JSON object"));
    std::unique_ptr<json_t, JanssonDeleter> obj{json_p, JanssonDeleter()};
    for (const auto& va : objectValue) {
        auto jval = dump_internal(va.get_value());
        std::string bytes;
        va.get_name().toUTF8String(bytes);
        auto err = json_object_set(obj.get(), bytes.c_str(), jval.get());
        if (err) throw SerializationException(SERIALIZATION_TRACEMSG(std::string(
                "Error setting field to JSON object, field name:[").append(bytes).append("]")));
    }
    return obj;
}
#else
std::unique_ptr<json_t, JanssonDeleter> dump_object(const std::vector<sr::ReflectedField>& objectValue) {
    auto json_p = json_object();
    if (!json_p) throw SerializationException(SERIALIZATION_TRACEMSG("Error initializing JSON object"));
    std::unique_ptr<json_t, JanssonDeleter> obj{json_p, JanssonDeleter()};
    for (const auto& va : objectValue) {
        auto jval = dump_internal(va.get_value());
        auto err = json_object_set(obj.get(), va.get_name().c_str(), jval.get());
        if (err) throw SerializationException(SERIALIZATION_TRACEMSG(std::string(
                "Error setting field to JSON object, field name:[").append(va.get_name()).append("]")));
    }
    return obj;
}
#endif // STATICLIB_WITH_ICU

std::unique_ptr<json_t, JanssonDeleter> dump_array(const std::vector<sr::ReflectedValue>& arrayValue) {
    auto json_p = json_array();
    if (!json_p) throw SerializationException(SERIALIZATION_TRACEMSG("Error initializing JSON array"));
    std::unique_ptr<json_t, JanssonDeleter> arr{json_p, JanssonDeleter()};
    for(const auto& va : arrayValue) {
        auto jval = dump_internal(va);
        auto err = json_array_append(arr.get(), jval.get());
        if (err) throw SerializationException(SERIALIZATION_TRACEMSG(
                "Error appending to JSON array, input size:[")
                .append(serialization_to_string(arrayValue.size()).append("]")));                
    }
    return arr;
}

#ifdef STATICLIB_WITH_ICU
std::unique_ptr<json_t, JanssonDeleter> dump_string(const icu::UnicodeString& stringValue) {
    std::string bytes{};
    stringValue.toUTF8String(bytes);
    auto json_p = json_string(bytes.c_str());
    if (!json_p) throw SerializationException(SERIALIZATION_TRACEMSG(
            "Error initializing JSON with value:[").append(bytes).append("]"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}
#else
std::unique_ptr<json_t, JanssonDeleter> dump_string(const std::string& stringValue) {
    auto json_p = json_string(stringValue.c_str());
    if (!json_p) throw SerializationException(SERIALIZATION_TRACEMSG(
            "Error initializing JSON with value:[").append(stringValue).append("]"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}
#endif // STATICLIB_WITH_ICU

std::unique_ptr<json_t, JanssonDeleter> dump_integer(int64_t integerValue) {
    auto json_p = json_integer(integerValue);
    if (!json_p) throw SerializationException(SERIALIZATION_TRACEMSG(
            "Error initializing JSON with value:[").append(serialization_to_string(integerValue)).append("]"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}

std::unique_ptr<json_t, JanssonDeleter> dump_real(double realValue) {
    auto json_p = json_real(realValue);
    if (!json_p) throw SerializationException(SERIALIZATION_TRACEMSG(
            "Error initializing JSON with value:[").append(serialization_to_string(realValue)).append("]"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}

std::unique_ptr<json_t, JanssonDeleter> dump_boolean(bool booleanValue) {
    auto json_p = json_boolean(booleanValue);
    if (!json_p) throw SerializationException(SERIALIZATION_TRACEMSG(
            "Error initializing JSON with value:[").append(serialization_to_string(booleanValue)).append("]"));
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
    default: throw SerializationException(SERIALIZATION_TRACEMSG(std::string(
            "Unsupported JSON type:[").append(serialization_to_string(static_cast<char>(value.get_type()))).append("]")));
    }
}

#ifdef STATICLIB_WITH_ICU
icu::UnicodeString json_to_string(const std::unique_ptr<json_t, JanssonDeleter>& json) {
    auto json_bytes_p = json_dumps(json.get(), JSON_INDENT(4) | JSON_PRESERVE_ORDER);
    if (!json_bytes_p) throw SerializationException(SERIALIZATION_TRACEMSG(std::string(
            "Error dumping JSON type: [").append(serialization_to_string(json_typeof(json.get()))).append("]")));
    std::unique_ptr<char, serialization_free_deleter<char>> json_bytes{json_bytes_p, serialization_free_deleter<char>()};
    return icu::UnicodeString(json_bytes.get());
}
#else
std::string json_to_string(const std::unique_ptr<json_t, JanssonDeleter>& json) {
    auto json_bytes_p = json_dumps(json.get(), JSON_INDENT(4) | JSON_PRESERVE_ORDER);
    if (!json_bytes_p) throw SerializationException(SERIALIZATION_TRACEMSG(std::string(
            "Error dumping JSON type: [").append(serialization_to_string(json_typeof(json.get()))).append("]")));
    std::unique_ptr<char, serialization_free_deleter<char>> json_bytes{json_bytes_p, serialization_free_deleter<char>()};
    return std::string(json_bytes.get());
}
#endif // STATICLIB_WITH_ICU
    
} // namespace

namespace { // anonymous, loads part

// forward declaration
sr::ReflectedValue load_internal(json_t* value);

sr::ReflectedValue load_object(json_t* value) {
    std::vector<sr::ReflectedField> obj{};
    size_t size = json_object_size(value);
    obj.reserve(size);
    
    // https://github.com/akheron/jansson/blob/23b1b7ba9a6bfce36d6e42623146c815e6b4e234/src/dump.c#L302
#ifdef STATICLIB_WITH_ICU
    std::vector<std::pair<icu::UnicodeString, size_t>> keys{};
#else
    std::vector<std::pair<std::string, size_t>> keys{};
#endif // STATICLIB_WITH_ICU
    keys.reserve(size);
    
    auto iter = json_object_iter(value);
    while (iter) {
        auto serial = hashtable_iter_serial(iter);
        auto key = json_object_iter_key(iter);
        keys.emplace_back(key, serial);
        iter = json_object_iter_next(value, iter);
    }
#ifdef STATICLIB_WITH_ICU
    std::sort(keys.begin(), keys.end(), [](const std::pair<icu::UnicodeString, size_t>& left,
            const std::pair<icu::UnicodeString, size_t>& right) {
        return left.second < right.second;
    });
    for (auto&& pair : keys) {
        std::string bytes;
        pair.first.toUTF8String(bytes);
        auto va = json_object_get(value, bytes.c_str());
        obj.emplace_back(std::move(pair.first), load_internal(va));
    }
#else    
    std::sort(keys.begin(), keys.end(), [](const std::pair<std::string, size_t>& left, 
            const std::pair<std::string, size_t>& right) {
        return left.second < right.second;
    });
    for (auto&& pair : keys) {
        auto va = json_object_get(value, pair.first.c_str());
        obj.emplace_back(std::move(pair.first), load_internal(va));
    }
#endif // STATICLIB_WITH_ICU
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
    if(!st) throw SerializationException(SERIALIZATION_TRACEMSG(std::string(
            "Error getting string value from JSON, type:[")
            .append(serialization_to_string(json_typeof(value))).append("]")));
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
    default: throw SerializationException(SERIALIZATION_TRACEMSG(
            "Unsupported JSON type:[").append(serialization_to_string(type).append("]")));
    }
}

#ifdef STATICLIB_WITH_ICU
std::unique_ptr<json_t, JanssonDeleter> json_from_string(const icu::UnicodeString& str) {
    json_error_t error;
    auto flags = JSON_REJECT_DUPLICATES | JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK;
    std::string bytes{};
    str.toUTF8String(bytes);
    auto json_p = json_loads(bytes.c_str(), flags, &error);
    if (!json_p) throw SerializationException(SERIALIZATION_TRACEMSG(std::string(
            "Error parsing JSON: [").append(bytes).append("]")
            .append(" text: [").append(error.text).append("]")
            .append(" line: [").append(serialization_to_string(error.line)).append("]")
            .append(" column: [").append(serialization_to_string(error.column)).append("]")
            .append(" position: [").append(serialization_to_string(error.position)).append("]")));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}
#else
std::unique_ptr<json_t, JanssonDeleter> json_from_string(const std::string& str) {
    json_error_t error;
    auto flags = JSON_REJECT_DUPLICATES | JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK;
    auto json_p = json_loads(str.c_str(), flags, &error);
    if (!json_p) throw SerializationException(SERIALIZATION_TRACEMSG(std::string(
            "Error parsing JSON: [").append(str).append("]")
            .append(" text: [").append(error.text).append("]")
            .append(" line: [").append(serialization_to_string(error.line)).append("]")
            .append(" column: [").append(serialization_to_string(error.column)).append("]")
            .append(" position: [").append(serialization_to_string(error.position)).append("]")));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}
#endif // STATICLIB_WITH_ICU

} // namespace

void init() {
    json_object_seed(0);
}

#ifdef STATICLIB_WITH_ICU
icu::UnicodeString dumps_json(const sr::ReflectedValue& value) {
    auto json = dump_internal(value);
    return json_to_string(json);
}
#else
std::string dumps_json(const sr::ReflectedValue& value) {
    auto json = dump_internal(value);
    return json_to_string(json);
}
#endif // STATICLIB_WITH_ICU

#ifdef STATICLIB_WITH_ICU
sr::ReflectedValue loads_json(const icu::UnicodeString& str) {
    auto json = json_from_string(str);
    return load_internal(json.get());
}
#else
sr::ReflectedValue loads_json(const std::string& str) {
    auto json = json_from_string(str);
    return load_internal(json.get());
}
#endif // STATICLIB_WITH_ICU

}
} // namespace

