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

#include <cassert>
#include <iostream>
#include <vector>
#include <unordered_set>

#ifdef STATICLIB_WITH_ICU
#include <unicode/unistr.h>
#else
#include <string>
#endif // STATICLIB_WITH_ICU

#include "staticlib/utils.hpp"
#include "staticlib/reflection.hpp"

#include "staticlib/serialization/json.hpp"

namespace { // anonymous

namespace ss = staticlib::utils;
namespace sr = staticlib::reflection;
namespace sj = staticlib::serialization;

#ifdef STATICLIB_WITH_ICU
class UStringHasher {
public:
    size_t operator()(icu::UnicodeString value) const {
        return static_cast<size_t> (value.hashCode());
    }
};

static const icu::UnicodeString JSON_STR =
#else
static const std::string JSON_STR =
#endif // STATICLIB_WITH_ICU
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
    sj::init();
}

class TestReflInner {
public:    
    sr::ReflectedValue get_reflected_value() const {
        return {
            { "f42", 42 },
            { "fnullable", nullptr }
        };
    }
};

class TestRefl {
    int32_t f1 = 41;
#ifdef STATICLIB_WITH_ICU
    icu::UnicodeString f2{"42"};
#else
    std::string f2{"42"};
#endif // STATICLIB_WITH_ICU
    bool f3 = true;

public:    
    sr::ReflectedValue get_reflected_value() const {
        std::vector<sr::ReflectedValue> vec{};
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
    auto st = sj::dumps_json(tr.get_reflected_value());
    assert(JSON_STR == st);
}

void test_loads() {
    {
        auto rv = sj::loads_json("null");
        assert(sr::ReflectedType::NULL_T == rv.get_type());
    }
    {
        auto rv = sj::loads_json("42");
        assert(sr::ReflectedType::INTEGER == rv.get_type());
    }
    {
        auto rv = sj::loads_json("[1,2,3]i am a foobar");
        assert(sr::ReflectedType::ARRAY == rv.get_type());
        assert(3 == rv.get_array().size());
    }
    
    auto rv = sj::loads_json(JSON_STR);
    auto& obj = rv.get_object();
    assert(5 == obj.size());
#ifdef STATICLIB_WITH_ICU    
    std::unordered_set<icu::UnicodeString, UStringHasher> set{};
#else
    std::unordered_set<std::string> set{};
#endif // STATICLIB_WITH_ICU    
    for (const auto& fi : obj) {
        set.insert(fi.get_name());
        if (fi.get_name() == "f1") {
            assert(sr::ReflectedType::INTEGER == fi.get_value().get_type());
            assert(41 == fi.get_value().get_integer());
        } else if (fi.get_name() == "f2") {
            assert(sr::ReflectedType::STRING == fi.get_value().get_type());
            assert(fi.get_value().get_string() == "42");
        } else if (fi.get_name() == "f3") { 
            assert(sr::ReflectedType::BOOLEAN == fi.get_value().get_type());
            assert(fi.get_value().get_boolean());
        } else if (fi.get_name() == "f4") {
            assert(sr::ReflectedType::ARRAY == fi.get_value().get_type());
            assert(2 == fi.get_value().get_array().size());
            assert(sr::ReflectedType::INTEGER == fi.get_value().get_array()[0].get_type());
            assert(41 == fi.get_value().get_array()[0].get_integer());
            assert(sr::ReflectedType::STRING == fi.get_value().get_array()[1].get_type());
            assert(fi.get_value().get_array()[1].get_string() == "43");
        } else if (fi.get_name() == "f5") {
            assert(sr::ReflectedType::OBJECT == fi.get_value().get_type());
            assert(2 == fi.get_value().get_object().size());
            assert(fi.get_value().get_object()[0].get_name() == "f42");
            assert(sr::ReflectedType::INTEGER == fi.get_value().get_object()[0].get_value().get_type());
            assert(42 == fi.get_value().get_object()[0].get_value().get_integer());
            assert(fi.get_value().get_object()[1].get_name() == "fnullable");
            assert(sr::ReflectedType::NULL_T == fi.get_value().get_object()[1].get_value().get_type());
        }
    }
    assert(5 == set.size());
    
}

void test_preserve_order() {
    auto vec = std::vector<sr::ReflectedField>{};
    for(auto i = 0; i < (1<<10); i++) {
        vec.emplace_back(ss::to_string(i).c_str(), i);
    }
    auto rv = sr::ReflectedValue(std::move(vec));
    auto json = sj::dumps_json(rv);
    auto loaded = sj::loads_json(json);
    for (auto i = 0; i < (1<<10); i++) {
        assert(loaded.get_object()[i].get_name() == ss::to_string(i).c_str());
        assert(i == loaded.get_object()[i].get_value().get_integer());
    }
}

} // namespace

int main() {
    
    test_init();
    test_dumps();
    test_loads();
    test_preserve_order();
    
    return 0;
}

