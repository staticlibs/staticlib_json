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
 * File:   jansson_utils_test.cpp
 * Author: alex
 * 
 * Created on January 1, 2015, 6:18 PM
 */

#include "staticlib/serialization/json.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#ifdef STATICLIB_WITH_ICU
#include <unicode/unistr.h>
#endif // STATICLIB_WITH_ICU

#include "staticlib/config/assert.hpp"

#include "staticlib/config.hpp"


namespace sc = staticlib::config;
namespace ss = staticlib::serialization;

static const std::string JSON_STR =
        R"({
    "f1": 41,
    "f2": "42",
    "f3": true,
    "f4": [
        41,
        "43"
    ],
    "f5": {
        "f42": 42,
        "fnullable": null
    }
})";

#ifdef STATICLIB_WITH_ICU
class UStringHasher {
public:
    size_t operator()(icu::UnicodeString value) const {
        return static_cast<size_t> (value.hashCode());
    }
};

static const icu::UnicodeString JSON_USTR = icu::UnicodeString::fromUTF8(JSON_STR);
#endif // STATICLIB_WITH_ICU

void test_init() {
    ss::init();
}

class TestReflInner {
public:    
    ss::JsonValue get_reflected_value() const {
        return {
            { "f42", 42 },
            { "fnullable", nullptr }
        };
    }
};

class TestRefl {
    int32_t f1 = 41;
#ifdef STATICLIB_WITH_ICU
    icu::UnicodeString f2 = "42";
#else
    std::string f2 = "42";
#endif // STATICLIB_WITH_ICU
    bool f3 = true;

public:    
    ss::JsonValue get_reflected_value() const {
        std::vector<ss::JsonValue> vec{};
        vec.emplace_back(41);
        vec.emplace_back("43");
        return {
            { "f1", f1 },
            { "f2", f2 },
            { "f3", f3 },
            { "f4", std::move(vec) },
            { "f5", TestReflInner().get_reflected_value() }
        };
    }
};

void test_dumps() {
    TestRefl tr{};
    std::string st = ss::dump_json_to_string(tr.get_reflected_value());
    slassert(JSON_STR == st);
}

#ifdef STATICLIB_WITH_ICU
void test_dumpu() {
    TestRefl tr{};
    icu::UnicodeString st = ss::dump_json_to_ustring(tr.get_reflected_value());
    slassert(JSON_USTR == st);
}
#endif // STATICLIB_WITH_ICU

void test_loads() {
    {
        auto rv = ss::load_json_from_string("null");
        slassert(ss::JsonType::NULL_T == rv.get_type());
    }
    {
        auto rv = ss::load_json_from_string("42");
        slassert(ss::JsonType::INTEGER == rv.get_type());
    }
    {
        auto rv = ss::load_json_from_string("[1,2,3]i am a foobar");
        slassert(ss::JsonType::ARRAY == rv.get_type());
        slassert(3 == rv.get_array().size());
    }
    
    auto rv = ss::load_json_from_string(JSON_STR);
    auto& obj = rv.get_object();
    slassert(5 == obj.size());
#ifdef STATICLIB_WITH_ICU
    std::unordered_set<icu::UnicodeString, UStringHasher> set{};
#else
    std::unordered_set<std::string> set{};
#endif // STATICLIB_WITH_ICU    
    for (const auto& fi : obj) {
        set.insert(fi.get_name());
        if (fi.get_name() == "f1") {
            slassert(ss::JsonType::INTEGER == fi.get_value().get_type());
            slassert(41 == fi.get_value().get_integer());
        } else if (fi.get_name() == "f2") {
            slassert(ss::JsonType::STRING == fi.get_value().get_type());
            slassert(fi.get_value().get_string() == "42");
        } else if (fi.get_name() == "f3") { 
            slassert(ss::JsonType::BOOLEAN == fi.get_value().get_type());
            slassert(fi.get_value().get_boolean());
        } else if (fi.get_name() == "f4") {
            slassert(ss::JsonType::ARRAY == fi.get_value().get_type());
            slassert(2 == fi.get_value().get_array().size());
            slassert(ss::JsonType::INTEGER == fi.get_value().get_array()[0].get_type());
            slassert(41 == fi.get_value().get_array()[0].get_integer());
            slassert(ss::JsonType::STRING == fi.get_value().get_array()[1].get_type());
            slassert(fi.get_value().get_array()[1].get_string() == "43");
        } else if (fi.get_name() == "f5") {
            slassert(ss::JsonType::OBJECT == fi.get_value().get_type());
            slassert(2 == fi.get_value().get_object().size());
            slassert(fi.get_value().get_object()[0].get_name() == "f42");
            slassert(ss::JsonType::INTEGER == fi.get_value().get_object()[0].get_value().get_type());
            slassert(42 == fi.get_value().get_object()[0].get_value().get_integer());
            slassert(fi.get_value().get_object()[1].get_name() == "fnullable");
            slassert(ss::JsonType::NULL_T == fi.get_value().get_object()[1].get_value().get_type());
        }
    }
    slassert(5 == set.size());
    
}

void test_preserve_order() {
    auto vec = std::vector<ss::JsonField>{};
    for(auto i = 0; i < (1<<10); i++) {
        vec.emplace_back(sc::to_string(i).c_str(), i);
    }
    auto rv = ss::JsonValue(std::move(vec));
    auto json = ss::dump_json_to_string(rv);
    auto loaded = ss::load_json_from_string(json);
    for (auto i = 0; i < (1<<10); i++) {
        slassert(loaded.get_object()[i].get_name() == sc::to_string(i).c_str());
        slassert(i == loaded.get_object()[i].get_value().get_integer());
    }
}

void test_dump_string() {
    ss::JsonValue val{"Not Found"};
    slassert(ss::JsonType::STRING == val.get_type());
    auto dumped = ss::dump_json_to_string(val);
    slassert("\"Not Found\"" == dumped);    
}

int main() {
    try {
        test_init();
        test_dumps();
#ifdef STATICLIB_WITH_ICU    
        test_dumpu();
#endif // STATICLIB_WITH_ICU        
        test_loads();
        test_preserve_order();
        test_dump_string();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

