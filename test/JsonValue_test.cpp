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

namespace ss = staticlib::serialization;

class TestRefl {
    int32_t f1 = 41;
    std::string f2 = "42";
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
    slassert(ss::JsonType::NULL_T == rv.type());
    slassert(0 == rv.as_object().size());
    slassert(0 == rv.as_array().size());
    slassert(0 == rv.as_string().length());
    slassert(0 == rv.as_int64());
    slassert(-1 < rv.as_double() && rv.as_double() < 1);
    slassert(!rv.as_bool());
    // setters
    slassert(!rv.set_bool(true));
    slassert(!rv.set_int64(42));
    slassert(!rv.set_double(42.0));
    slassert(!rv.set_string("foo"));
    // mutators
    slassert(!rv.as_object_mutable().second);
    slassert(!rv.as_array_mutable().second);
}

void test_object() {
    ss::JsonValue rv = TestRefl{}.get_reflected_value().clone();
    slassert(ss::JsonType::OBJECT == rv.type());
    slassert(6 == rv.as_object().size());
    slassert("f1" == rv.as_object()[0].name());
    slassert(41 == rv.as_object()[0].value().as_int64());
    slassert("f2" == rv.as_object()[1].name());
    slassert("42" == rv.as_object()[1].as_string());
    slassert(ss::JsonType::INTEGER == rv.as_object()[0].value().type());
    slassert(ss::JsonType::STRING == rv.as_object()[1].value().type());
    slassert(ss::JsonType::BOOLEAN == rv.as_object()[2].value().type());
    slassert(ss::JsonType::ARRAY == rv.as_object()[3].value().type());
    slassert(ss::JsonType::OBJECT == rv.as_object()[4].value().type());    
    slassert(ss::JsonType::NULL_T == rv.as_object()[5].value().type());
    slassert(0 == rv.as_array().size());
    slassert(0 == rv.as_string().length());
    slassert(0 == rv.as_int64());
    slassert(-1 < rv.as_double() && rv.as_double() < 1);
    slassert(!rv.as_bool());
    // mutators
    slassert(rv.as_object_mutable().second);
    slassert(6 == rv.as_object_mutable().first->size());
    rv.as_object_mutable().first->emplace_back("added", "aaa");
    slassert(7 == rv.as_object().size());
    slassert("aaa" == rv.as_object()[6].as_string());
    slassert(!rv.as_array_mutable().second);
    // setters
    slassert(!rv.set_bool(true));
    slassert(!rv.set_int64(42));
    slassert(!rv.set_double(42.0));
    slassert(!rv.set_string("foo"));
}

void test_array() {
    std::vector<ss::JsonValue> vec{};
    vec.emplace_back(42);
    vec.emplace_back(true);
    ss::JsonValue rv{std::move(vec)};
    slassert(ss::JsonType::ARRAY == rv.type());
    slassert(0 == rv.as_object().size());
    slassert(2 == rv.as_array().size());
    slassert(42 == rv.as_array()[0].as_int64());
    slassert(rv.as_array()[1].as_bool());
    slassert(0 == rv.as_string().length());
    slassert(0 == rv.as_int64());
    slassert(-1 < rv.as_double() && rv.as_double() < 1);
    slassert(!rv.as_bool());
    // mutators
    slassert(!rv.as_object_mutable().second);
    slassert(rv.as_array_mutable().second);
    slassert(2 == rv.as_array_mutable().first->size());
    rv.as_array_mutable().first->emplace_back("aaa");
    slassert(3 == rv.as_array().size());
    slassert("aaa" == rv.as_array()[2].as_string());
    // setters
    slassert(!rv.set_bool(true));
    slassert(!rv.set_int64(42));
    slassert(!rv.set_double(42.0));
    slassert(!rv.set_string("foo"));
}

void test_string() {
    ss::JsonValue rv{"42"};
    slassert(ss::JsonType::STRING == rv.type());
    slassert(0 == rv.as_object().size());
    slassert(0 == rv.as_array().size());
    slassert(2 == rv.as_string().length());
    slassert("42" == rv.as_string());
    slassert(0 == rv.as_int64());
    slassert(-1 < rv.as_double() && rv.as_double() < 1);
    slassert(!rv.as_bool());
    // mutators
    slassert(!rv.as_object_mutable().second);
    slassert(!rv.as_array_mutable().second);
    // setters
    slassert(rv.set_string("foo"));
    slassert("foo" == rv.as_string());
    slassert(!rv.set_bool(true));
    slassert(!rv.set_int64(42));
    slassert(!rv.set_double(42.0));
    // copy
    const std::string str{"43"};
    ss::JsonValue rvc{str};
    slassert("43" == rvc.as_string());
}

void test_string_default() {
    ss::JsonValue rv{};
    (void) rv;
    slassert("42" == rv.as_string("42"));
}

void test_int() {
    ss::JsonValue rv{42};
    slassert(ss::JsonType::INTEGER == rv.type());
    slassert(0 == rv.as_object().size());
    slassert(0 == rv.as_array().size());
    slassert(0 == rv.as_string().length());
    slassert(42 == rv.as_int64());
    slassert(-1 < rv.as_double() && rv.as_double() < 1);
    slassert(!rv.as_bool());
    // mutators
    slassert(!rv.as_object_mutable().second);
    slassert(!rv.as_array_mutable().second);
    // setters
    slassert(rv.set_int64(43));
    slassert(43 == rv.as_int64());
    slassert(!rv.set_bool(true));
    slassert(!rv.set_double(42.0));
    slassert(!rv.set_string("foo"));
}

void test_int_default() {
    ss::JsonValue rv{};
    slassert(42 == rv.as_int64(42));
}

void test_real() {
    ss::JsonValue rv{42.0};
    slassert(ss::JsonType::REAL == rv.type());
    slassert(0 == rv.as_object().size());
    slassert(0 == rv.as_array().size());
    slassert(0 == rv.as_string().length());
    slassert(0 == rv.as_int64());
    slassert(41 < rv.as_double() && rv.as_double() < 43);
    slassert(41 < rv.as_float() && rv.as_float() < 43);    
    slassert(!rv.as_bool());
    // mutators
    slassert(!rv.as_object_mutable().second);
    slassert(!rv.as_array_mutable().second);
    // setters
    slassert(rv.set_double(43.0));
    slassert(42 < rv.as_double() && rv.as_double() < 44);
    slassert(rv.set_float(43.0));
    slassert(42 < rv.as_float() && rv.as_float() < 44);
    slassert(!rv.set_bool(true));
    slassert(!rv.set_int64(42));
    slassert(!rv.set_string("foo"));
}

void test_real_default() {
    ss::JsonValue rv{};
    slassert(41 < rv.as_double(42) && rv.as_double(42) < 43);
}

void test_boolean() {
    ss::JsonValue rvt{true};
    slassert(ss::JsonType::BOOLEAN == rvt.type());
    slassert(0 == rvt.as_object().size());
    slassert(0 == rvt.as_array().size());
    slassert(0 == rvt.as_string().length());
    slassert(0 == rvt.as_int64());
    slassert(-1 < rvt.as_double() && rvt.as_double() < 1);
    slassert(rvt.as_bool());
    // setters
    slassert(rvt.set_bool(false));
    slassert(!rvt.as_bool());
    slassert(!rvt.set_int64(42));
    slassert(!rvt.set_double(43.0));
    slassert(!rvt.set_string("foo"));
    // mutators
    slassert(!rvt.as_object_mutable().second);
    slassert(!rvt.as_array_mutable().second);

    ss::JsonValue rvf{false};
    slassert(ss::JsonType::BOOLEAN == rvf.type());
    slassert(0 == rvf.as_object().size());
    slassert(0 == rvf.as_array().size());
    slassert(0 == rvf.as_string().length());
    slassert(0 == rvf.as_int64());
    slassert(-1 < rvf.as_double() && rvf.as_double() < 1);
    slassert(!rvf.as_bool());
    // setters
    slassert(rvf.set_bool(true));
    slassert(rvf.as_bool());
    slassert(!rvf.set_int64(42));
    slassert(!rvf.set_double(43.0));
    slassert(!rvf.set_string("foo"));
    // mutators
    slassert(!rvf.as_object_mutable().second);
    slassert(!rvf.as_array_mutable().second);
}

void test_boolean_default() {
    ss::JsonValue rv{};
    slassert(rv.as_bool(true));
    slassert(!rv.as_bool(false));
}

void test_from_range() {
    std::vector<std::string> vec{"foo", "bar"};
    
    auto rv = ss::JsonValue(vec);
    slassert(ss::JsonType::ARRAY == rv.type());
}

void test_field_by_name() {
    TestRefl tr{};
    auto rv = tr.get_reflected_value();
    
    auto& rvf = rv["transient_f4"];
    slassert(ss::JsonType::ARRAY == rvf.type());
    slassert(2 == rvf.as_array().size());
    slassert(42 == rvf.as_array()[0].as_int64());
    slassert("foo" == rvf.as_array()[1].as_string());
    
    auto& rv_null = rv["aaa"];
    slassert(ss::JsonType::NULL_T == rv_null.type());
}

#ifdef STATICLIB_WITH_ICU
void test_icu() {
    icu::UnicodeString st{"foo"};
    ss::JsonValue val{st};
    slassert(icu::UnicodeString{"foo"} == val.as_ustring());
    val.set_ustring("bar");
    slassert(icu::UnicodeString{"bar"} == val.as_ustring());
}
#endif // STATICLIB_WITH_ICU

void test_mutable() {
    TestRefl tr{};
    auto rv = tr.get_reflected_value();
    
    // get existing
    slassert(41 == rv["f1"].as_int32());
    slassert(41 == rv.getattr("f1").as_int32());
    slassert(41 == rv.getattr_mutable("f1").as_int32());
#ifdef STATICLIB_WITH_ICU
    slassert(41 == rv.getattru("f1").as_int32());
    slassert(41 == rv.getattru_mutable("f1").as_int32());
#endif // STATICLIB_WITH_ICU
    
    // create new attr
    ss::JsonValue& nval = rv.getattr_mutable("foo");
    slassert(ss::JsonType::NULL_T == nval.type());
    nval.set_int32(42);
    slassert(ss::JsonType::INTEGER == nval.type());
    nval.set_string("foo");
    slassert(ss::JsonType::STRING == nval.type());
    nval.set_float(static_cast<float>(0.1));
    slassert(ss::JsonType::REAL == nval.type());
    nval.set_bool(false);
    slassert(ss::JsonType::BOOLEAN == nval.type());
    std::vector<ss::JsonValue> arrval{};
    arrval.emplace_back("bar");
    nval.set_array(std::move(arrval));
    slassert(ss::JsonType::ARRAY == nval.type());
    slassert("bar" == nval.as_array()[0].as_string());
    std::vector<ss::JsonField> objval{};
    objval.emplace_back("baz", 42);
    nval.set_object(std::move(objval));
    slassert(ss::JsonType::OBJECT == nval.type());
    slassert("baz" == nval.as_object()[0].name());
    slassert(42 == nval.as_object()[0].value().as_int32());
    
    // convert to object
    ss::JsonValue& exval = rv.getattr_mutable("f1");
    slassert(ss::JsonType::INTEGER == exval.type());
    ss::JsonValue& crval = exval.getattr_mutable("foo");
    slassert(ss::JsonType::OBJECT == exval.type());
    slassert(ss::JsonType::NULL_T == crval.type());
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
#ifdef STATICLIB_WITH_ICU
        test_icu();
#endif // STATICLIB_WITH_ICU
        test_mutable();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
