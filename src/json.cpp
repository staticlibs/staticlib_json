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
#include <array>
#include <cstddef>

#ifdef STATICLIB_WITH_ICU
#include <unicode/unistr.h>
#endif // STATICLIB_WITH_ICU

#include "jansson.h"
#include "jansson_hashtable_helper.h"

// for older janssons
#ifndef json_array_foreach
#define json_array_foreach(array, index, value) \
	for(index = 0; \
		index < json_array_size(array) && (value = json_array_get(array, index)); \
		index++)
#endif // json_array_foreach

#include "staticlib/utils.hpp"
#include "staticlib/icu_utils.hpp"
#include "staticlib/io.hpp"

#include "staticlib/serialization/SerializationException.hpp"
#include "staticlib/serialization/json.hpp"

#include "JanssonDeleter.hpp"

namespace staticlib {
namespace serialization {

namespace ss = staticlib::utils;
namespace iu = staticlib::icu_utils;
namespace io = staticlib::io;
namespace sr = staticlib::reflection;

namespace { // anonymous, dump part

// forward declaration
std::unique_ptr<json_t, JanssonDeleter> dump_internal(const sr::ReflectedValue& value);

std::unique_ptr<json_t, JanssonDeleter> dump_null() {
    auto json_p = json_null();
    if (!json_p) throw SerializationException(TRACEMSG("Error initializing JSON null"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}

#ifdef STATICLIB_WITH_ICU
std::unique_ptr<json_t, JanssonDeleter> dump_object(const std::vector<sr::ReflectedField>& objectValue) {
    auto json_p = json_object();
    if (!json_p) throw SerializationException(TRACEMSG("Error initializing JSON object"));
    std::unique_ptr<json_t, JanssonDeleter> obj{json_p, JanssonDeleter()};
    for (const auto& va : objectValue) {
        auto jval = dump_internal(va.get_value());
        std::string bytes;
        va.get_name().toUTF8String(bytes);
        auto err = json_object_set(obj.get(), bytes.c_str(), jval.get());
        if (err) throw SerializationException(TRACEMSG(std::string{} + 
                "Error setting field to JSON object, field name:[" + bytes + "]"));
    }
    return obj;
}
#else
std::unique_ptr<json_t, JanssonDeleter> dump_object(const std::vector<sr::ReflectedField>& objectValue) {
    auto json_p = json_object();
    if (!json_p) throw SerializationException(TRACEMSG("Error initializing JSON object"));
    std::unique_ptr<json_t, JanssonDeleter> obj{json_p, JanssonDeleter()};
    for (const auto& va : objectValue) {
        auto jval = dump_internal(va.get_value());
        auto err = json_object_set(obj.get(), va.get_name().c_str(), jval.get());
        if (err) throw SerializationException(TRACEMSG(std::string{} +
                "Error setting field to JSON object, field name:[" + va.get_name() + "]"));
    }
    return obj;
}
#endif // STATICLIB_WITH_ICU

std::unique_ptr<json_t, JanssonDeleter> dump_array(const std::vector<sr::ReflectedValue>& arrayValue) {
    auto json_p = json_array();
    if (!json_p) throw SerializationException(TRACEMSG("Error initializing JSON array"));
    std::unique_ptr<json_t, JanssonDeleter> arr{json_p, JanssonDeleter()};
    for (const auto& va : arrayValue) {
        auto jval = dump_internal(va);
        auto err = json_array_append(arr.get(), jval.get());
        if (err) throw SerializationException(TRACEMSG(
                "Error appending to JSON array, input size:[" + ss::to_string(arrayValue.size()) + "]"));
    }
    return arr;
}

#ifdef STATICLIB_WITH_ICU
std::unique_ptr<json_t, JanssonDeleter> dump_string(const icu::UnicodeString& stringValue) {
    std::string bytes{};
    stringValue.toUTF8String(bytes);
    auto json_p = json_string(bytes.c_str());
    if (!json_p) throw SerializationException(TRACEMSG(
            "Error initializing JSON with value:[" + bytes + "]"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}
#else
std::unique_ptr<json_t, JanssonDeleter> dump_string(const std::string& stringValue) {
    auto json_p = json_string(stringValue.c_str());
    if (!json_p) throw SerializationException(TRACEMSG(
            "Error initializing JSON with value:[" + stringValue + "]"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}
#endif // STATICLIB_WITH_ICU

std::unique_ptr<json_t, JanssonDeleter> dump_integer(int64_t integerValue) {
    auto json_p = json_integer(integerValue);
    if (!json_p) throw SerializationException(TRACEMSG(
            "Error initializing JSON with value:[" + ss::to_string(integerValue) + "]"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}

std::unique_ptr<json_t, JanssonDeleter> dump_real(double realValue) {
    auto json_p = json_real(realValue);
    if (!json_p) throw SerializationException(TRACEMSG(
            "Error initializing JSON with value:[" + ss::to_string(realValue) + "]"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
}

std::unique_ptr<json_t, JanssonDeleter> dump_boolean(bool booleanValue) {
    auto json_p = booleanValue? json_true() : json_false();
    if (!json_p) throw SerializationException(TRACEMSG(
            "Error initializing JSON with value:[" + ss::to_string(booleanValue) + "]"));
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
    default: throw SerializationException(TRACEMSG(std::string{} +
                "Unsupported JSON type:[" + ss::to_string(static_cast<char> (value.get_type())) + "]"));
    }
}

class Dumper {
    io::streambuf_sink dest;
    std::string error;

public:
    Dumper(std::streambuf& dest) :
    dest(std::addressof(dest)) { }

    Dumper(const Dumper&) = delete;

    Dumper& operator=(const Dumper&) = delete;

    int write(const char* buffer, size_t size) {
        size_t result = 0;
        while (result < size) {
            auto amt = dest.write(buffer + result, size - result);
            result += static_cast<size_t> (amt);
        }
        return 0;
    }

    void set_error(const std::string& err) {
        this->error = err;
    }

    const std::string& get_error() {
        return error;
    }
};

int dump_callback(const char* buffer, size_t size, void* data) {
    Dumper* dumper = static_cast<Dumper*>(data);
    try {
        return dumper->write(buffer, size);
    } catch(const std::exception& e) {
        dumper->set_error(e.what());
        return -1;
    }
}

void json_to_streambuf(json_t* json, std::streambuf& dest) {
    Dumper dumper{dest};
    void* dumper_ptr = static_cast<void*>(std::addressof(dumper));
    int res = json_dump_callback(json, dump_callback, dumper_ptr, size_t JSON_INDENT(4) | JSON_PRESERVE_ORDER);
    if (0 != res) throw SerializationException(TRACEMSG(std::string{} +
            "Error dumping JSON type: [" + ss::to_string(json_typeof(json)) + "],"
            " error: [" + dumper.get_error() + "]"));
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
#ifdef STATICLIB_WITH_ICU
    std::vector<std::pair < icu::UnicodeString, size_t >> keys{};
#else
    std::vector<std::pair < std::string, size_t >> keys{};
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
    size_t i;
    json_t* va;

    json_array_foreach(value, i, va) {
        arr.push_back(load_internal(va));
    }
    return sr::ReflectedValue(std::move(arr));
}

sr::ReflectedValue load_string(json_t* value) {
    auto st = json_string_value(value);
    if (!st) throw SerializationException(TRACEMSG(std::string{} +
            "Error getting string value from JSON, type:[" + ss::to_string(json_typeof(value)) + "]"));
    return sr::ReflectedValue(st);
}

sr::ReflectedValue load_internal(json_t* value) {
    json_type type = json_typeof(value);
    switch (type) {
    case (JSON_NULL): return sr::ReflectedValue();
    case (JSON_OBJECT): return load_object(value);
    case (JSON_ARRAY): return load_array(value);
    case (JSON_STRING): return load_string(value);
    case (JSON_INTEGER): return sr::ReflectedValue(static_cast<int64_t> (json_integer_value(value)));
    case (JSON_REAL): return sr::ReflectedValue(json_real_value(value));
    case (JSON_TRUE): return sr::ReflectedValue(true);
    case (JSON_FALSE): return sr::ReflectedValue(false);
    default: throw SerializationException(TRACEMSG(
                "Unsupported JSON type:[" + ss::to_string(type) + "]"));
    }
}

class Loader {
    io::streambuf_source src;
    std::string error;

public:
    Loader(std::streambuf& src) :
    src(std::addressof(src)) { }

    Loader(const Loader&) = delete;

    Loader& operator=(const Loader&) = delete;

    int read(char* buffer, size_t size) {
        std::streamsize res = 0;
        while (0 == res) {
            res = src.read(buffer, size);
        }
        return std::char_traits<char>::eof() != res ? static_cast<int>(res) : 0;
    }

    void set_error(const std::string& err) {
        this->error = err;
    }

    const std::string& get_error() {
        return error;
    }
};

size_t load_callback(void* buffer, size_t buflen, void *data) {
    Loader* loader = static_cast<Loader*> (data);
    try {
        return loader->read(static_cast<char*>(buffer), buflen);
    } catch (const std::exception& e) {
        loader->set_error(e.what());
        return static_cast<size_t>(-1);
    }
}

std::unique_ptr<json_t, JanssonDeleter> json_from_streambuf(std::streambuf& src) {
#if JANSSON_VERSION_HEX >= 0x020400
    Loader loader{src};
    void* ldr_ptr = static_cast<void*>(std::addressof(loader));
    json_error_t error;
    auto flags = JSON_REJECT_DUPLICATES | JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK;
    auto json_p = json_load_callback(load_callback, ldr_ptr, flags, std::addressof(error));
    if (!json_p) throw SerializationException(TRACEMSG(std::string{} +
    "Error parsing JSON:" +
            " text: [" + error.text + "]" +
            " line: [" + ss::to_string(error.line) + "]" +
            " column: [" + ss::to_string(error.column) + "]" +
            " position: [" + ss::to_string(error.position) + "],"
            " callback error: [" + loader.get_error() + "]"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
#else
    io::streambuf_source bufsrc{std::addressof(src)};
    io::string_sink sink{};
    std::array<char, 8192> buf;    
    io::copy_all(bufsrc, sink, buf.data(), buf.size());
    json_error_t error;
    auto flags = JSON_REJECT_DUPLICATES | JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK;
    auto json_p = json_loads(sink.get_string().c_str(), flags, std::addressof(error));
    if (!json_p) throw SerializationException(TRACEMSG(std::string{} +
            "Error parsing JSON: [" + sink.get_string() + "]" +
            " text: [" + error.text + "]" +
            " line: [" + ss::to_string(error.line) + "]" +
            " column: [" + ss::to_string(error.column) + "]"+
            " position: [" + ss::to_string(error.position) + "]"));
    return std::unique_ptr<json_t, JanssonDeleter>{json_p, JanssonDeleter()};
#endif    
}

} // namespace

void init() {
#if JANSSON_VERSION_HEX >= 0x020600
    json_object_seed(0);
#endif    
}

void dump_json(const sr::ReflectedValue& value, std::streambuf& dest) {
    auto json = dump_internal(value);
    json_to_streambuf(json.get(), dest);
}

std::string dump_json_to_string(const sr::ReflectedValue& value) {
    auto json = dump_internal(value);
    auto streambuf = io::make_unbuffered_ostreambuf(io::string_sink{});
    json_to_streambuf(json.get(), streambuf);
    return std::move(streambuf.get_sink().get_string());
}

#ifdef STATICLIB_WITH_ICU
icu::UnicodeString dump_json_to_ustring(const sr::ReflectedValue& value) {
    auto json = dump_internal(value);
    auto streambuf = io::make_unbuffered_ostreambuf(iu::ustring_sink{});
    json_to_streambuf(json.get(), streambuf);
    return std::move(streambuf.get_sink().get_string());
}
#endif // STATICLIB_WITH_ICU

sr::ReflectedValue load_json(std::streambuf& src) {
    auto json = json_from_streambuf(src);
    return load_internal(json.get());
}

sr::ReflectedValue load_json_from_string(const std::string& str) {
    auto streambuf = io::make_unbuffered_istreambuf(io::array_source{str.data(), str.size()});
    auto json = json_from_streambuf(streambuf);
    return load_internal(json.get());
}

#ifdef STATICLIB_WITH_ICU
sr::ReflectedValue load_json_from_ustring(const icu::UnicodeString& str) {
    auto streambuf = io::make_unbuffered_istreambuf(iu::uarray_source{str});
    auto json = json_from_streambuf(streambuf);
    return load_internal(json.get());
}
#endif // STATICLIB_WITH_ICU

}
} // namespace

