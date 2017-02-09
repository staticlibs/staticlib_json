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

#include "staticlib/serialization/json_operations.hpp"

#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <cstddef>
#include <cstdint>

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

#include "staticlib/config.hpp"
#ifdef STATICLIB_WITH_ICU
#include "staticlib/icu_utils.hpp"
#endif // STATICLIB_WITH_ICU
#include "staticlib/io.hpp"

#include "staticlib/serialization/json_field.hpp"
#include "staticlib/serialization/serialization_exception.hpp"


namespace staticlib {
namespace serialization {

namespace sc = staticlib::config;
#ifdef STATICLIB_WITH_ICU
namespace iu = staticlib::icu_utils;
#endif // STATICLIB_WITH_ICU
namespace io = staticlib::io;

namespace { // anonymous, dump part

class jansson_deleter {
public:
    void operator()(json_t* json) {
        json_decref(json);
    }
};

// forward declaration
std::unique_ptr<json_t, jansson_deleter> dump_internal(const json_value& value);

std::unique_ptr<json_t, jansson_deleter> dump_null() {
    auto json_p = json_null();
    if (!json_p) throw serialization_exception(TRACEMSG("Error initializing JSON null"));
    return std::unique_ptr<json_t, jansson_deleter>{json_p, jansson_deleter()};
}

std::unique_ptr<json_t, jansson_deleter> dump_object(const std::vector<json_field>& objectValue) {
    auto json_p = json_object();
    if (!json_p) throw serialization_exception(TRACEMSG("Error initializing JSON object"));
    std::unique_ptr<json_t, jansson_deleter> obj{json_p, jansson_deleter()};
    for (const auto& va : objectValue) {
        auto jval = dump_internal(va.value());
        auto err = json_object_set(obj.get(), va.name().c_str(), jval.get());
        if (err) throw serialization_exception(TRACEMSG(
                "Error setting field to JSON object, field name:[" + va.name() + "]"));
    }
    return obj;
}

std::unique_ptr<json_t, jansson_deleter> dump_array(const std::vector<json_value>& arrayValue) {
    auto json_p = json_array();
    if (!json_p) throw serialization_exception(TRACEMSG("Error initializing JSON array"));
    std::unique_ptr<json_t, jansson_deleter> arr{json_p, jansson_deleter()};
    for (const auto& va : arrayValue) {
        auto jval = dump_internal(va);
        auto err = json_array_append(arr.get(), jval.get());
        if (err) throw serialization_exception(TRACEMSG(
                "Error appending to JSON array, input size:[" + sc::to_string(arrayValue.size()) + "]"));
    }
    return arr;
}

std::unique_ptr<json_t, jansson_deleter> dump_string(const std::string& stringValue) {
    auto json_p = json_string(stringValue.c_str());
    if (!json_p) throw serialization_exception(TRACEMSG(
            "Error initializing JSON with value:[" + stringValue + "]"));
    return std::unique_ptr<json_t, jansson_deleter>{json_p, jansson_deleter()};
}

std::unique_ptr<json_t, jansson_deleter> dump_integer(int64_t integerValue) {
    auto json_p = json_integer(integerValue);
    if (!json_p) throw serialization_exception(TRACEMSG(
            "Error initializing JSON with value:[" + sc::to_string(integerValue) + "]"));
    return std::unique_ptr<json_t, jansson_deleter>{json_p, jansson_deleter()};
}

std::unique_ptr<json_t, jansson_deleter> dump_real(double realValue) {
    auto json_p = json_real(realValue);
    if (!json_p) throw serialization_exception(TRACEMSG(
            "Error initializing JSON with value:[" + sc::to_string(realValue) + "]"));
    return std::unique_ptr<json_t, jansson_deleter>{json_p, jansson_deleter()};
}

std::unique_ptr<json_t, jansson_deleter> dump_boolean(bool booleanValue) {
    auto json_p = booleanValue? json_true() : json_false();
    if (!json_p) throw serialization_exception(TRACEMSG(
            "Error initializing JSON with value:[" + sc::to_string(booleanValue) + "]"));
    return std::unique_ptr<json_t, jansson_deleter>{json_p, jansson_deleter()};
}

std::unique_ptr<json_t, jansson_deleter> dump_internal(const json_value& value) {
    switch (value.type()) {
    case (json_type::nullt): return dump_null();
    case (json_type::object): return dump_object(value.as_object());
    case (json_type::array): return dump_array(value.as_array());
    case (json_type::string): return dump_string(value.as_string());
    case (json_type::integer): return dump_integer(value.as_int64());
    case (json_type::real): return dump_real(value.as_double());
    case (json_type::boolean): return dump_boolean(value.as_bool());
    default: throw serialization_exception(TRACEMSG(
                "Unsupported JSON type:[" + sc::to_string(static_cast<char> (value.type())) + "]"));
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
            auto amt = dest.write({buffer + result, size - result});
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
    int res = json_dump_callback(json, dump_callback, dumper_ptr, JSON_ENCODE_ANY | JSON_INDENT(4) | JSON_PRESERVE_ORDER);
    if (0 != res) throw serialization_exception(TRACEMSG(
            "Error dumping JSON type: [" + sc::to_string(json_typeof(json)) + "],"
            " error: [" + dumper.get_error() + "]"));
}

} // namespace

namespace { // anonymous, loads part

// forward declaration
json_value load_internal(json_t* value);

json_value load_object(json_t* value) {
    std::vector<json_field> obj{};
    size_t size = json_object_size(value);
    obj.reserve(size);

// https://github.com/akheron/jansson/blob/23b1b7ba9a6bfce36d6e42623146c815e6b4e234/src/dump.c#L302
    std::vector<std::pair < std::string, size_t >> keys{};
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
    return json_value(std::move(obj));
}

json_value load_array(json_t* value) {
    std::vector<json_value> arr{};
    arr.reserve(json_array_size(value));
    size_t i;
    json_t* va;

    json_array_foreach(value, i, va) {
        arr.push_back(load_internal(va));
    }
    return json_value(std::move(arr));
}

json_value load_string(json_t* value) {
    auto st = json_string_value(value);
    if (!st) throw serialization_exception(TRACEMSG(
            "Error getting string value from JSON, type:[" + sc::to_string(json_typeof(value)) + "]"));
    return json_value(st);
}

json_value load_internal(json_t* value) {
    ::json_type type = json_typeof(value);
    switch (type) {
    case (JSON_NULL): return json_value();
    case (JSON_OBJECT): return load_object(value);
    case (JSON_ARRAY): return load_array(value);
    case (JSON_STRING): return load_string(value);
    case (JSON_INTEGER): return json_value(static_cast<int64_t> (json_integer_value(value)));
    case (JSON_REAL): return json_value(json_real_value(value));
    case (JSON_TRUE): return json_value(true);
    case (JSON_FALSE): return json_value(false);
    default: throw serialization_exception(TRACEMSG(
                "Unsupported JSON type:[" + sc::to_string(type) + "]"));
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
            res = src.read({buffer, size});
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

#if JANSSON_VERSION_HEX >= 0x020400
size_t load_callback(void* buffer, size_t buflen, void *data) {
    Loader* loader = static_cast<Loader*> (data);
    try {
        return loader->read(static_cast<char*>(buffer), buflen);
    } catch (const std::exception& e) {
        loader->set_error(e.what());
        return static_cast<size_t>(-1);
    }
}
#endif // JANSSON_VERSION_HEX >= 0x020400

std::unique_ptr<json_t, jansson_deleter> json_from_streambuf(std::streambuf& src) {
#if JANSSON_VERSION_HEX >= 0x020400
    Loader loader{src};
    void* ldr_ptr = static_cast<void*>(std::addressof(loader));
    json_error_t error;
    auto flags = JSON_REJECT_DUPLICATES | JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK;
    auto json_p = json_load_callback(load_callback, ldr_ptr, flags, std::addressof(error));
    if (!json_p) throw serialization_exception(TRACEMSG("Error parsing JSON:" +
            " text: [" + error.text + "]" +
            " line: [" + sc::to_string(error.line) + "]" +
            " column: [" + sc::to_string(error.column) + "]" +
            " position: [" + sc::to_string(error.position) + "],"
            " callback error: [" + loader.get_error() + "]"));
    return std::unique_ptr<json_t, jansson_deleter>{json_p, jansson_deleter()};
#else
    io::streambuf_source bufsrc{std::addressof(src)};
    io::string_sink sink{};
    std::array<char, 8192> buf;    
    io::copy_all(bufsrc, sink, buf.data(), buf.size());
    json_error_t error;
    auto flags = JSON_REJECT_DUPLICATES | JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK;
    auto json_p = json_loads(sink.get_string().c_str(), flags, std::addressof(error));
    if (!json_p) throw serialization_exception(TRACEMSG("Error parsing JSON: [" + sink.get_string() + "]" +
            " text: [" + error.text + "]" +
            " line: [" + sc::to_string(error.line) + "]" +
            " column: [" + sc::to_string(error.column) + "]"+
            " position: [" + sc::to_string(error.position) + "]"));
    return std::unique_ptr<json_t, jansson_deleter>{json_p, jansson_deleter()};
#endif    
}

} // namespace

void init() {
#if JANSSON_VERSION_HEX >= 0x020600
    json_object_seed(0);
#endif    
}

void dump_json_to_streambuf(const json_value& value, std::streambuf& dest) {
    auto json = dump_internal(value);
    json_to_streambuf(json.get(), dest);
}

std::string dump_json_to_string(const json_value& value) {
    auto json = dump_internal(value);
    auto streambuf = io::make_unbuffered_ostreambuf(io::string_sink{});
    json_to_streambuf(json.get(), streambuf);
    return std::move(streambuf.get_sink().get_string());
}

#ifdef STATICLIB_WITH_ICU
icu::UnicodeString dump_json_to_ustring(const json_value& value) {
    auto json = dump_internal(value);
    auto streambuf = io::make_unbuffered_ostreambuf(iu::ustring_sink{});
    json_to_streambuf(json.get(), streambuf);
    return std::move(streambuf.get_sink().get_string());
}
#endif // STATICLIB_WITH_ICU

json_value load_json_from_streambuf(std::streambuf& src) {
    auto json = json_from_streambuf(src);
    return load_internal(json.get());
}

json_value load_json_from_string(const std::string& str) {
    auto streambuf = io::make_unbuffered_istreambuf(io::array_source{str.data(), str.size()});
    auto json = json_from_streambuf(streambuf);
    return load_internal(json.get());
}

#ifdef STATICLIB_WITH_ICU
json_value load_json_from_ustring(const icu::UnicodeString& str) {
    auto streambuf = io::make_unbuffered_istreambuf(iu::uarray_source{str});
    auto json = json_from_streambuf(streambuf);
    return load_internal(json.get());
}
#endif // STATICLIB_WITH_ICU

const json_value& null_json() {
    static json_value empty;
    return empty;
}

}
} // namespace
