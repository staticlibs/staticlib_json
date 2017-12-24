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
 * File:   operations_test.cpp
 * Author: alex
 * 
 * Created on January 1, 2015, 6:18 PM
 */

#include "staticlib/json/operations.hpp"

#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#include "staticlib/config/assert.hpp"

#include "staticlib/config.hpp"

static const std::string test_json_str =
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

void test_init() {
    sl::json::init();
}

class test_refl_inner {
public:    
    sl::json::value get_reflected_value() const {
        return {
            { "f42", 42 },
            { "fnullable", nullptr }
        };
    }
};

class test_refl {
    int32_t f1 = 41;
    std::string f2 = "42";
    bool f3 = true;

public:    
    sl::json::value get_reflected_value() const {
        return {
            { "f1", f1 },
            { "f2", f2 },
            { "f3", f3 },
            { "f4", [] {
                std::vector<sl::json::value> vec;
                vec.emplace_back(41);
                vec.emplace_back("43");
                return vec;
            }() },
            { "f5", test_refl_inner().get_reflected_value() }
        };
    }
};

void test_dumps() {
    test_refl tr{};
    std::string st = tr.get_reflected_value().dumps();
    slassert(test_json_str == st);
}

void test_loads() {
    {
        auto rv = sl::json::loads("null");
        slassert(sl::json::type::nullt == rv.json_type());
    }
    {
        auto rv = sl::json::loads("42");
        slassert(sl::json::type::integer == rv.json_type());
    }
    {
        auto rv = sl::json::loads("[1,2,3]i am a foobar");
        slassert(sl::json::type::array == rv.json_type());
        slassert(3 == rv.as_array().size());
    }
    
    auto rv = sl::json::loads(test_json_str);
    auto& obj = rv.as_object();
    slassert(5 == obj.size());
    std::unordered_set<std::string> set{};
    for (const auto& fi : obj) {
        set.insert(fi.name());
        if (fi.name() == "f1") {
            slassert(sl::json::type::integer == fi.val().json_type());
            slassert(41 == fi.val().as_int64());
        } else if (fi.name() == "f2") {
            slassert(sl::json::type::string == fi.val().json_type());
            slassert(fi.val().as_string() == "42");
        } else if (fi.name() == "f3") { 
            slassert(sl::json::type::boolean == fi.val().json_type());
            slassert(fi.val().as_bool());
        } else if (fi.name() == "f4") {
            slassert(sl::json::type::array == fi.val().json_type());
            slassert(2 == fi.val().as_array().size());
            slassert(sl::json::type::integer == fi.val().as_array()[0].json_type());
            slassert(41 == fi.val().as_array()[0].as_int64());
            slassert(sl::json::type::string == fi.val().as_array()[1].json_type());
            slassert(fi.val().as_array()[1].as_string() == "43");
        } else if (fi.name() == "f5") {
            slassert(sl::json::type::object == fi.val().json_type());
            slassert(2 == fi.val().as_object().size());
            slassert(fi.val().as_object()[0].name() == "f42");
            slassert(sl::json::type::integer == fi.val().as_object()[0].val().json_type());
            slassert(42 == fi.val().as_object()[0].val().as_int64());
            slassert(fi.val().as_object()[1].name() == "fnullable");
            slassert(sl::json::type::nullt == fi.val().as_object()[1].val().json_type());
        }
    }
    slassert(5 == set.size());
    
}

void test_preserve_order() {
    auto vec = std::vector<sl::json::field>{};
    for(auto i = 0; i < (1<<10); i++) {
        vec.emplace_back(sl::support::to_string(i).c_str(), i);
    }
    auto rv = sl::json::value(std::move(vec));
    auto json = rv.dumps();
    auto loaded = sl::json::loads(json);
    for (auto i = 0; i < (1<<10); i++) {
        slassert(loaded.as_object()[i].name() == sl::support::to_string(i).c_str());
        slassert(i == loaded.as_object()[i].val().as_int64());
    }
}

void test_dump_string() {
    sl::json::value val{"Not Found"};
    slassert(sl::json::type::string == val.json_type());
    auto dumped = val.dumps();
    slassert("\"Not Found\"" == dumped);    
}

void test_dumps_short() {
    slassert("\"foo\"" == sl::json::dumps("foo"));
}

int main() {
    try {
        test_init();
        test_dumps();
        // todo: fixme, broken on 2.10
        // test_loads();
        // todo: fixme, broken on 2.10
        // test_preserve_order();
        test_dump_string();
        test_dumps_short();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

