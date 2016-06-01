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
 * File:   ReflectedValue_test.cpp
 * Author: alex
 *
 * Created on December 28, 2014, 9:38 PM
 */

#include "staticlib/serialization/JsonValue.hpp"

#include <iostream>

#include "staticlib/config/assert.hpp"

#include "staticlib/serialization/JsonField.hpp"

#ifdef STATICLIB_WITH_ICU
#define TEST_STR(CHARS) icu::UnicodeString{CHARS}
#else
#define TEST_STR(CHARS) std::string{CHARS}
#endif // STATICLIB_WITH_ICU

namespace ss = staticlib::serialization;

class TestRefl {
    int32_t f1 = 41;
#ifdef STATICLIB_WITH_ICU
    icu::UnicodeString f2 = "42";
#else    
    std::string f2 = "42";
#endif    
    bool f3 = true;

public:
    ss::JsonValue get_reflected_value() const {
        std::vector<ss::JsonValue> vec{};
        vec.emplace_back(42);
        vec.emplace_back("foo");
        return {
            {"f1", f1},
            {"f2", f2},
            {"f3", f3},
            {"transient_f4", std::move(vec)},
            {"transient_f5", {
                {"key1", "val1"},
                {"key2", "val2"}
            }},
            {"fnullable", nullptr}
        };
    }
};

void test_null() { 
    ss::JsonValue rv{};
    slassert(ss::JsonType::NULL_T == rv.get_type());
    slassert(0 == rv.get_object().size());
    slassert(0 == rv.get_array().size());
    slassert(0 == rv.get_string().length());
    slassert(0 == rv.get_integer());
    slassert(-1 < rv.get_real() && rv.get_real() < 1);
    slassert(!rv.get_boolean());
    // setters
    slassert(!rv.set_boolean(true));
    slassert(!rv.set_integer(42));
    slassert(!rv.set_real(42.0));
    slassert(!rv.set_string("foo"));
    // mutators
    slassert(!rv.get_object_ptr().second);
    slassert(!rv.get_array_ptr().second);
}

void test_object() {
    ss::JsonValue rv = TestRefl{}.get_reflected_value().clone();
    slassert(ss::JsonType::OBJECT == rv.get_type());
    slassert(6 == rv.get_object().size());
    slassert(TEST_STR("f1") == rv.get_object()[0].get_name());
    slassert(41 == rv.get_object()[0].get_value().get_integer());
    slassert(TEST_STR("f2") == rv.get_object()[1].get_name());
    slassert(TEST_STR("42") == rv.get_object()[1].get_string());
    slassert(ss::JsonType::INTEGER == rv.get_object()[0].get_value().get_type());
    slassert(ss::JsonType::STRING == rv.get_object()[1].get_value().get_type());
    slassert(ss::JsonType::BOOLEAN == rv.get_object()[2].get_value().get_type());
    slassert(ss::JsonType::ARRAY == rv.get_object()[3].get_value().get_type());
    slassert(ss::JsonType::OBJECT == rv.get_object()[4].get_value().get_type());    
    slassert(ss::JsonType::NULL_T == rv.get_object()[5].get_value().get_type());
    slassert(0 == rv.get_array().size());
    slassert(0 == rv.get_string().length());
    slassert(0 == rv.get_integer());
    slassert(-1 < rv.get_real() && rv.get_real() < 1);
    slassert(!rv.get_boolean());
    // setters
    slassert(!rv.set_boolean(true));
    slassert(!rv.set_integer(42));
    slassert(!rv.set_real(42.0));
    slassert(!rv.set_string("foo"));
    // mutators
    slassert(rv.get_object_ptr().second);
    slassert(6 == rv.get_object_ptr().first->size());
    rv.get_object_ptr().first->emplace_back("added", "aaa");
    slassert(7 == rv.get_object().size());
    slassert(TEST_STR("aaa") == rv.get_object()[6].get_string());
    slassert(!rv.get_array_ptr().second);
}

void test_array() {
    std::vector<ss::JsonValue> vec{};
    vec.emplace_back(42);
    vec.emplace_back(true);
    ss::JsonValue rv{std::move(vec)};
    slassert(ss::JsonType::ARRAY == rv.get_type());
    slassert(0 == rv.get_object().size());
    slassert(2 == rv.get_array().size());
    slassert(42 == rv.get_array()[0].get_integer());
    slassert(rv.get_array()[1].get_boolean());
    slassert(0 == rv.get_string().length());
    slassert(0 == rv.get_integer());
    slassert(-1 < rv.get_real() && rv.get_real() < 1);
    slassert(!rv.get_boolean());
    // setters
    slassert(!rv.set_boolean(true));
    slassert(!rv.set_integer(42));
    slassert(!rv.set_real(42.0));
    slassert(!rv.set_string("foo"));
    // mutators
    slassert(!rv.get_object_ptr().second);
    slassert(rv.get_array_ptr().second);
    slassert(2 == rv.get_array_ptr().first->size());
    rv.get_array_ptr().first->emplace_back("aaa");
    slassert(3 == rv.get_array().size());
    slassert(TEST_STR("aaa") == rv.get_array()[2].get_string());
}

void test_string() {
    ss::JsonValue rv{"42"};
    slassert(ss::JsonType::STRING == rv.get_type());
    slassert(0 == rv.get_object().size());
    slassert(0 == rv.get_array().size());
    slassert(2 == rv.get_string().length());
    slassert(TEST_STR("42") == rv.get_string());
    slassert(0 == rv.get_integer());
    slassert(-1 < rv.get_real() && rv.get_real() < 1);
    slassert(!rv.get_boolean());
    // setters
    slassert(!rv.set_boolean(true));
    slassert(!rv.set_integer(42));
    slassert(!rv.set_real(42.0));
    slassert(rv.set_string("foo"));
    slassert(TEST_STR("foo") == rv.get_string());
    // mutators
    slassert(!rv.get_object_ptr().second);
    slassert(!rv.get_array_ptr().second);
}

void test_string_default() {
    ss::JsonValue rv{};
    (void) rv;
    slassert(TEST_STR("42") == rv.get_string("42"));
}

void test_int() {
    ss::JsonValue rv{42};
    slassert(ss::JsonType::INTEGER == rv.get_type());
    slassert(0 == rv.get_object().size());
    slassert(0 == rv.get_array().size());
    slassert(0 == rv.get_string().length());
    slassert(42 == rv.get_integer());
    slassert(-1 < rv.get_real() && rv.get_real() < 1);
    slassert(!rv.get_boolean());
    // setters
    slassert(!rv.set_boolean(true));
    slassert(rv.set_integer(43));
    slassert(43 == rv.get_integer());
    slassert(!rv.set_real(42.0));
    slassert(!rv.set_string("foo"));
    // mutators
    slassert(!rv.get_object_ptr().second);
    slassert(!rv.get_array_ptr().second);
}

void test_int_default() {
    ss::JsonValue rv{};
    slassert(42 == rv.get_integer(42));
}

void test_real() {
    ss::JsonValue rv{42.0};
    slassert(ss::JsonType::REAL == rv.get_type());
    slassert(0 == rv.get_object().size());
    slassert(0 == rv.get_array().size());
    slassert(0 == rv.get_string().length());
    slassert(0 == rv.get_integer());
    slassert(41 < rv.get_real() && rv.get_real() < 43);
    slassert(!rv.get_boolean());
    // setters
    slassert(!rv.set_boolean(true));
    slassert(!rv.set_integer(42));
    slassert(rv.set_real(43.0));
    slassert(42 < rv.get_real() && rv.get_real() < 44);
    slassert(!rv.set_string("foo"));
    // mutators
    slassert(!rv.get_object_ptr().second);
    slassert(!rv.get_array_ptr().second);
}

void test_real_default() {
    ss::JsonValue rv{};
    slassert(41 < rv.get_real(42) && rv.get_real(42) < 43);
}

void test_boolean() {
    ss::JsonValue rvt{true};
    slassert(ss::JsonType::BOOLEAN == rvt.get_type());
    slassert(0 == rvt.get_object().size());
    slassert(0 == rvt.get_array().size());
    slassert(0 == rvt.get_string().length());
    slassert(0 == rvt.get_integer());
    slassert(-1 < rvt.get_real() && rvt.get_real() < 1);
    slassert(rvt.get_boolean());
    // setters
    slassert(rvt.set_boolean(false));
    slassert(!rvt.get_boolean());
    slassert(!rvt.set_integer(42));
    slassert(!rvt.set_real(43.0));
    slassert(!rvt.set_string("foo"));
    // mutators
    slassert(!rvt.get_object_ptr().second);
    slassert(!rvt.get_array_ptr().second);

    ss::JsonValue rvf{false};
    slassert(ss::JsonType::BOOLEAN == rvf.get_type());
    slassert(0 == rvf.get_object().size());
    slassert(0 == rvf.get_array().size());
    slassert(0 == rvf.get_string().length());
    slassert(0 == rvf.get_integer());
    slassert(-1 < rvf.get_real() && rvf.get_real() < 1);
    slassert(!rvf.get_boolean());
    // setters
    slassert(rvf.set_boolean(true));
    slassert(rvf.get_boolean());
    slassert(!rvf.set_integer(42));
    slassert(!rvf.set_real(43.0));
    slassert(!rvf.set_string("foo"));
    // mutators
    slassert(!rvf.get_object_ptr().second);
    slassert(!rvf.get_array_ptr().second);
}

void test_boolean_default() {
    ss::JsonValue rv{};
    slassert(rv.get_boolean(true));
    slassert(!rv.get_boolean(false));
}

void test_from_range() {
#ifdef STATICLIB_WITH_ICU
    std::vector<icu::UnicodeString> vec{"foo", "bar"};
#else    
    std::vector<std::string> vec{"foo", "bar"};
#endif // STATICLIB_WITH_ICU    
    
    auto rv = ss::JsonValue(vec);
    slassert(ss::JsonType::ARRAY == rv.get_type());
}

void test_field_by_name() {
    TestRefl tr{};
    auto rv = tr.get_reflected_value();
    
    auto& rvf = rv.get("transient_f4");
    slassert(ss::JsonType::ARRAY == rvf.get_type());
    slassert(2 == rvf.get_array().size());
    slassert(42 == rvf.get_array()[0].get_integer());
    slassert(TEST_STR("foo") == rvf.get_array()[1].get_string());
    
    auto& rv_null = rv.get("aaa");
    slassert(ss::JsonType::NULL_T == rv_null.get_type());
}

int main() {
    try {
        test_null();
        test_object();
        test_array();
        test_string();
        test_string_default();
        test_int();
        test_int_default();
        test_real();
        test_real_default();
        test_boolean();
        test_boolean_default();
        test_from_range();
        test_field_by_name();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
