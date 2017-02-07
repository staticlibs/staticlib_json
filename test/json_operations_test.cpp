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

#include "staticlib/serialization/json_operations.hpp"

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
static const icu::UnicodeString JSON_USTR = icu::UnicodeString::fromUTF8(JSON_STR);
#endif // STATICLIB_WITH_ICU

void test_init() {
    ss::init();
}

class TestReflInner {
public:    
    ss::json_value get_reflected_value() const {
        return {
            { "f42", 42 },
            { "fnullable", nullptr }
        };
    }
};

class TestRefl {
    int32_t f1 = 41;
    std::string f2 = "42";
    bool f3 = true;

public:    
    ss::json_value get_reflected_value() const {
        std::vector<ss::json_value> vec{};
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
        slassert(ss::json_type::NULL_T == rv.type());
    }
    {
        auto rv = ss::load_json_from_string("42");
        slassert(ss::json_type::INTEGER == rv.type());
    }
    {
        auto rv = ss::load_json_from_string("[1,2,3]i am a foobar");
        slassert(ss::json_type::ARRAY == rv.type());
        slassert(3 == rv.as_array().size());
    }
    
    auto rv = ss::load_json_from_string(JSON_STR);
    auto& obj = rv.as_object();
    slassert(5 == obj.size());
    std::unordered_set<std::string> set{};
    for (const auto& fi : obj) {
        set.insert(fi.name());
        if (fi.name() == "f1") {
            slassert(ss::json_type::INTEGER == fi.value().type());
            slassert(41 == fi.value().as_int64());
        } else if (fi.name() == "f2") {
            slassert(ss::json_type::STRING == fi.value().type());
            slassert(fi.value().as_string() == "42");
        } else if (fi.name() == "f3") { 
            slassert(ss::json_type::BOOLEAN == fi.value().type());
            slassert(fi.value().as_bool());
        } else if (fi.name() == "f4") {
            slassert(ss::json_type::ARRAY == fi.value().type());
            slassert(2 == fi.value().as_array().size());
            slassert(ss::json_type::INTEGER == fi.value().as_array()[0].type());
            slassert(41 == fi.value().as_array()[0].as_int64());
            slassert(ss::json_type::STRING == fi.value().as_array()[1].type());
            slassert(fi.value().as_array()[1].as_string() == "43");
        } else if (fi.name() == "f5") {
            slassert(ss::json_type::OBJECT == fi.value().type());
            slassert(2 == fi.value().as_object().size());
            slassert(fi.value().as_object()[0].name() == "f42");
            slassert(ss::json_type::INTEGER == fi.value().as_object()[0].value().type());
            slassert(42 == fi.value().as_object()[0].value().as_int64());
            slassert(fi.value().as_object()[1].name() == "fnullable");
            slassert(ss::json_type::NULL_T == fi.value().as_object()[1].value().type());
        }
    }
    slassert(5 == set.size());
    
}

void test_preserve_order() {
    auto vec = std::vector<ss::json_field>{};
    for(auto i = 0; i < (1<<10); i++) {
        vec.emplace_back(sc::to_string(i).c_str(), i);
    }
    auto rv = ss::json_value(std::move(vec));
    auto json = ss::dump_json_to_string(rv);
    auto loaded = ss::load_json_from_string(json);
    for (auto i = 0; i < (1<<10); i++) {
        slassert(loaded.as_object()[i].name() == sc::to_string(i).c_str());
        slassert(i == loaded.as_object()[i].value().as_int64());
    }
}

void test_dump_string() {
    ss::json_value val{"Not Found"};
    slassert(ss::json_type::STRING == val.type());
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

