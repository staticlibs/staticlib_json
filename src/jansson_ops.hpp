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
 * File:   jansson_ops.hpp
 * Author: alex
 *
 * Created on April 9, 2017, 8:41 PM
 */

#ifndef STATICLIB_JSON_JANSSON_OPS_HPP
#define STATICLIB_JSON_JANSSON_OPS_HPP

#include <cstddef>
#include <cstdint>
#include <algorithm>
#include <array>
#include <memory>
#include <string>
#include <vector>
#include <utility>

#include "jansson.h"
#include "jansson_hashtable_helper.hpp"

// for older janssons
#ifndef json_array_foreach
#define json_array_foreach(array, index, value) \
 for(index = 0; \
  index < json_array_size(array) && (value = json_array_get(array, index)); \
  index++)
#endif // json_array_foreach

#include "staticlib/config.hpp"
#include "staticlib/support.hpp"
#include "staticlib/io.hpp"

#include "staticlib/json/field.hpp"
#include "staticlib/json/json_exception.hpp"

#include "jansson_deleter.hpp"

namespace staticlib {
namespace json {

namespace detail_dump {

// forward declaration
inline std::unique_ptr<json_t, jansson_deleter> dump_internal(const value& value);

inline std::unique_ptr<json_t, jansson_deleter> dump_null() {
    auto json_p = json_null();
    if (!json_p) throw json_exception(TRACEMSG("Error initializing JSON null"));
    return std::unique_ptr<json_t, jansson_deleter>{json_p, jansson_deleter()};
}

inline std::unique_ptr<json_t, jansson_deleter> dump_object(const std::vector<field>& objectValue) {
    auto json_p = json_object();
    if (!json_p) throw json_exception(TRACEMSG("Error initializing JSON object"));
    std::unique_ptr<json_t, jansson_deleter> obj{json_p, jansson_deleter()};
    for (const auto& va : objectValue) {
        auto jval = dump_internal(va.val());
        auto err = json_object_set(obj.get(), va.name().c_str(), jval.get());
        if (err) throw json_exception(TRACEMSG(
                "Error setting field to JSON object, field name:[" + va.name() + "]"));
    }
    return obj;
}

inline std::unique_ptr<json_t, jansson_deleter> dump_array(const std::vector<value>& arrayValue) {
    auto json_p = json_array();
    if (!json_p) throw json_exception(TRACEMSG("Error initializing JSON array"));
    std::unique_ptr<json_t, jansson_deleter> arr{json_p, jansson_deleter()};
    for (const auto& va : arrayValue) {
        auto jval = dump_internal(va);
        auto err = json_array_append(arr.get(), jval.get());
        if (err) throw json_exception(TRACEMSG(
                "Error appending to JSON array, input size:[" + sl::support::to_string(arrayValue.size()) + "]"));
    }
    return arr;
}

inline std::unique_ptr<json_t, jansson_deleter> dump_string(const std::string& stringValue) {
    auto json_p = json_string(stringValue.c_str());
    if (!json_p) throw json_exception(TRACEMSG(
            "Error initializing JSON with value:[" + stringValue + "]"));
    return std::unique_ptr<json_t, jansson_deleter>{json_p, jansson_deleter()};
}

inline std::unique_ptr<json_t, jansson_deleter> dump_integer(int64_t integerValue) {
    auto json_p = json_integer(integerValue);
    if (!json_p) throw json_exception(TRACEMSG(
            "Error initializing JSON with value:[" + sl::support::to_string(integerValue) + "]"));
    return std::unique_ptr<json_t, jansson_deleter>{json_p, jansson_deleter()};
}

inline std::unique_ptr<json_t, jansson_deleter> dump_real(double realValue) {
    auto json_p = json_real(realValue);
    if (!json_p) throw json_exception(TRACEMSG(
            "Error initializing JSON with value:[" + sl::support::to_string(realValue) + "]"));
    return std::unique_ptr<json_t, jansson_deleter>{json_p, jansson_deleter()};
}

inline std::unique_ptr<json_t, jansson_deleter> dump_boolean(bool booleanValue) {
    auto json_p = booleanValue ? json_true() : json_false();
    if (!json_p) throw json_exception(TRACEMSG(
            "Error initializing JSON with value:[" + sl::support::to_string(booleanValue) + "]"));
    return std::unique_ptr<json_t, jansson_deleter>{json_p, jansson_deleter()};
}

inline std::unique_ptr<json_t, jansson_deleter> dump_internal(const value& value) {
    switch (value.json_type()) {
    case (type::nullt): return dump_null();
    case (type::object): return dump_object(value.as_object());
    case (type::array): return dump_array(value.as_array());
    case (type::string): return dump_string(value.as_string());
    case (type::integer): return dump_integer(value.as_int64());
    case (type::real): return dump_real(value.as_double());
    case (type::boolean): return dump_boolean(value.as_bool());
    default: throw json_exception(TRACEMSG(
                "Unsupported JSON type:[" + stringify_json_type(value.json_type()) + "]"));
    }
}

class dumper {
    sl::io::streambuf_sink dest;
    std::string error;

public:

    dumper(std::streambuf& dest) :
    dest(std::addressof(dest)) { }

    dumper(const dumper&) = delete;

    dumper& operator=(const dumper&) = delete;

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

inline int dump_callback(const char* buffer, size_t size, void* data) {
    dumper* dmp = static_cast<dumper*> (data);
    try {
        return dmp->write(buffer, size);
    } catch (const std::exception& e) {
        dmp->set_error(e.what());
        return -1;
    }
}

inline void json_to_streambuf(json_t* json, std::streambuf& dest) {
    dumper dmp{dest};
    void* dumper_ptr = static_cast<void*> (std::addressof(dmp));
    int res = json_dump_callback(json, dump_callback, dumper_ptr, JSON_ENCODE_ANY | JSON_INDENT(4) | JSON_PRESERVE_ORDER);
    if (0 != res) throw json_exception(TRACEMSG(
            "Error dumping JSON type: [" + sl::support::to_string(json_typeof(json)) + "],"
            " error: [" + dmp.get_error() + "]"));
}

} // namespace

namespace detail_load {

// forward declaration
value load_internal(json_t* jvalue);

inline value load_object(json_t* object_value) {
    std::vector<field> obj{};
    size_t size = json_object_size(object_value);
    obj.reserve(size);

    // https://github.com/akheron/jansson/blob/23b1b7ba9a6bfce36d6e42623146c815e6b4e234/src/dump.c#L302
    std::vector<std::pair < std::string, size_t >> keys
    {
    };
    keys.reserve(size);

    auto iter = json_object_iter(object_value);
    while (iter) {
        auto serial = hashtable_iter_serial(iter);
        auto key = json_object_iter_key(iter);
        keys.emplace_back(key, serial);
        iter = json_object_iter_next(object_value, iter);
    }
    std::sort(keys.begin(), keys.end(), [](const std::pair<std::string, size_t>& left,
            const std::pair<std::string, size_t>& right) {
        return left.second < right.second;
    });
    for (auto&& pair : keys) {
        auto va = json_object_get(object_value, pair.first.c_str());
        obj.emplace_back(std::move(pair.first), load_internal(va));
    }
    return value(std::move(obj));
}

inline value load_array(json_t* array_value) {
    std::vector<value> arr{};
    arr.reserve(json_array_size(array_value));
    size_t i;
    json_t* va;

    json_array_foreach(array_value, i, va) {
        arr.push_back(load_internal(va));
    }
    return value(std::move(arr));
}

inline value load_string(json_t* string_value) {
    auto st = json_string_value(string_value);
    if (!st) throw json_exception(TRACEMSG(
            "Error getting string value from JSON, type:[" + sl::support::to_string(json_typeof(string_value)) + "]"));
    return value(st);
}

inline value load_internal(json_t* jvalue) {
    ::json_type type = json_typeof(jvalue);
    switch (type) {
    case (JSON_NULL): return value();
    case (JSON_OBJECT): return load_object(jvalue);
    case (JSON_ARRAY): return load_array(jvalue);
    case (JSON_STRING): return load_string(jvalue);
    case (JSON_INTEGER): return value(static_cast<int64_t> (json_integer_value(jvalue)));
    case (JSON_REAL): return value(json_real_value(jvalue));
    case (JSON_TRUE): return value(true);
    case (JSON_FALSE): return value(false);
    default: throw json_exception(TRACEMSG(
                "Unsupported JSON type:[" + sl::support::to_string(type) + "]"));
    }
}

class loader {
    sl::io::streambuf_source src;
    std::string error;

public:

    loader(std::streambuf& src) :
    src(std::addressof(src)) { }

    loader(const loader&) = delete;

    loader& operator=(const loader&) = delete;

    int read(char* buffer, size_t size) {
        std::streamsize res = 0;
        while (0 == res) {
            res = src.read({buffer, size});
        }
        return std::char_traits<char>::eof() != res ? static_cast<int> (res) : 0;
    }

    void set_error(const std::string& err) {
        this->error = err;
    }

    const std::string& get_error() {
        return error;
    }
};

#if JANSSON_VERSION_HEX >= 0x020400

inline size_t load_callback(void* buffer, size_t buflen, void *data) {
    loader* ldr = static_cast<loader*> (data);
    try {
        return ldr->read(static_cast<char*> (buffer), buflen);
    } catch (const std::exception& e) {
        ldr->set_error(e.what());
        return static_cast<size_t> (-1);
    }
}
#endif // JANSSON_VERSION_HEX >= 0x020400

inline std::unique_ptr<json_t, jansson_deleter> json_from_streambuf(std::streambuf& src) {
#if JANSSON_VERSION_HEX >= 0x020400
    loader loader{src};
    void* ldr_ptr = static_cast<void*> (std::addressof(loader));
    json_error_t error;
    auto flags = JSON_REJECT_DUPLICATES | JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK;
    auto json_p = json_load_callback(load_callback, ldr_ptr, flags, std::addressof(error));
    if (!json_p) throw json_exception(TRACEMSG("Error parsing JSON:" +
            " text: [" + error.text + "]" +
            " line: [" + sl::support::to_string(error.line) + "]" +
            " column: [" + sl::support::to_string(error.column) + "]" +
            " position: [" + sl::support::to_string(error.position) + "],"
            " callback error: [" + loader.get_error() + "]"));
    return std::unique_ptr<json_t, jansson_deleter>{json_p, jansson_deleter()};
#else
    sl::io::streambuf_source bufsrc{std::addressof(src)};
    sl::io::string_sink sink{};
    sl::io::copy_all(bufsrc, sink);
    json_error_t error;
    auto flags = JSON_REJECT_DUPLICATES | JSON_DECODE_ANY | JSON_DISABLE_EOF_CHECK;
    auto json_p = json_loads(sink.get_string().c_str(), flags, std::addressof(error));
    if (!json_p) throw json_exception(TRACEMSG("Error parsing JSON: [" + sink.get_string() + "]" +
            " text: [" + error.text + "]" +
            " line: [" + sl::support::to_string(error.line) + "]" +
            " column: [" + sl::support::to_string(error.column) + "]" +
            " position: [" + sl::support::to_string(error.position) + "]"));
    return std::unique_ptr<json_t, jansson_deleter>{json_p, jansson_deleter()};
#endif    
}

} // namespace

inline void jansson_dump_to_streambuf(const value& value, std::streambuf* dest) {
    auto json = detail_dump::dump_internal(value);
    detail_dump::json_to_streambuf(json.get(), *dest);
}

inline std::string jansson_dump_to_string(const value& value) {
    auto json = detail_dump::dump_internal(value);
    auto streambuf = sl::io::make_unbuffered_ostreambuf(io::string_sink{});
    detail_dump::json_to_streambuf(json.get(), streambuf);
    return std::move(streambuf.get_sink().get_string());
}

inline value jansson_load_from_streambuf(std::streambuf* src) {
    auto json = detail_load::json_from_streambuf(*src);
    return detail_load::load_internal(json.get());
}

inline value jansson_load_from_string(const std::string& str) {
    auto streambuf = sl::io::make_unbuffered_istreambuf(io::array_source{str.data(), str.size()});
    auto json = detail_load::json_from_streambuf(streambuf);
    return detail_load::load_internal(json.get());
}

inline void jansson_init() {
#if JANSSON_VERSION_HEX >= 0x020600
    json_object_seed(0);
#endif    
}

}
} // namespace

#endif /* STATICLIB_JSON_JANSSON_OPS_HPP */

