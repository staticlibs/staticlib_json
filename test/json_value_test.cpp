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

#include "staticlib/serialization/json_value.hpp"

#include <iostream>
#include <functional>
#include <limits>

#include "staticlib/config/assert.hpp"

#include "staticlib/serialization/json_field.hpp"

namespace ss = staticlib::serialization;

bool throws_exc(std::function<void()> fun) {
    try {
        fun();
    } catch (const ss::serialization_exception& e) {
        (void) e;
        return true;
    }
    return false;
}

class TestRefl {
    int32_t f1 = 41;
    std::string f2 = "42";
    bool f3 = true;

public:
    ss::json_value get_reflected_value() const {
        std::vector<ss::json_value> vec{};
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
    ss::json_value rv{};
    slassert(ss::json_type::nullt == rv.type());
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
}

void test_object() {
    ss::json_value rv = TestRefl{}.get_reflected_value().clone();
    slassert(ss::json_type::object == rv.type());
    slassert(6 == rv.as_object().size());
    slassert("f1" == rv.as_object()[0].name());
    slassert(41 == rv.as_object()[0].value().as_int64());
    slassert("f2" == rv.as_object()[1].name());
    slassert("42" == rv.as_object()[1].as_string());
    slassert(ss::json_type::integer == rv.as_object()[0].value().type());
    slassert(ss::json_type::string == rv.as_object()[1].value().type());
    slassert(ss::json_type::boolean == rv.as_object()[2].value().type());
    slassert(ss::json_type::array == rv.as_object()[3].value().type());
    slassert(ss::json_type::object == rv.as_object()[4].value().type());    
    slassert(ss::json_type::nullt == rv.as_object()[5].value().type());
    slassert(0 == rv.as_array().size());
    slassert(0 == rv.as_string().length());
    slassert(0 == rv.as_int64());
    slassert(-1 < rv.as_double() && rv.as_double() < 1);
    slassert(!rv.as_bool());
    // mutators
    slassert(!rv.as_object_or_throw().empty());
    slassert(6 == rv.as_object_or_throw().size());
    rv.as_object_or_throw().emplace_back("added", "aaa");
    slassert(7 == rv.as_object().size());
    slassert("aaa" == rv.as_object()[6].as_string());
    // not object
    ss::json_value& exval = rv.getattr_or_throw("f1");
    slassert(ss::json_type::integer == exval.type());
    bool caught = throws_exc([&exval]{ exval.as_object_or_throw(); });
    slassert(caught);
    // setters
    slassert(!rv.set_bool(true));
    slassert(!rv.set_int64(42));
    slassert(!rv.set_double(42.0));
    slassert(!rv.set_string("foo"));
}

void test_array() {
    std::vector<ss::json_value> vec{};
    vec.emplace_back(42);
    vec.emplace_back(true);
    ss::json_value rv{std::move(vec)};
    slassert(ss::json_type::array == rv.type());
    slassert(0 == rv.as_object().size());
    slassert(2 == rv.as_array().size());
    slassert(42 == rv.as_array()[0].as_int64());
    slassert(rv.as_array()[1].as_bool());
    slassert(0 == rv.as_string().length());
    slassert(0 == rv.as_int64());
    slassert(-1 < rv.as_double() && rv.as_double() < 1);
    slassert(!rv.as_bool());
    // mutators
    slassert(!rv.as_array_or_throw().empty());
    slassert(2 == rv.as_array_or_throw().size());
    rv.as_array_or_throw().emplace_back("aaa");
    slassert(3 == rv.as_array().size());
    slassert("aaa" == rv.as_array()[2].as_string());
    // not array
    auto exval = ss::json_value(42);
    slassert(ss::json_type::integer == exval.type());
    bool caught = throws_exc([&exval] { exval.as_array_or_throw(); });
    slassert(caught);
    // setters
    slassert(!rv.set_bool(true));
    slassert(!rv.set_int64(42));
    slassert(!rv.set_double(42.0));
    slassert(!rv.set_string("foo"));
}

void test_string() {
    ss::json_value rv{"42"};
    slassert(ss::json_type::string == rv.type());
    slassert(0 == rv.as_object().size());
    slassert(0 == rv.as_array().size());
    slassert(2 == rv.as_string().length());
    slassert("42" == rv.as_string());
    slassert(0 == rv.as_int64());
    slassert(-1 < rv.as_double() && rv.as_double() < 1);
    slassert(!rv.as_bool());
    // mutable
    slassert("42" == rv.as_string_or_throw());
    std::string& ref = rv.as_string_or_throw();
    ref[1] = '3';
    slassert("43" == rv.as_string());
    slassert("43" == rv.as_string_or_throw());
    // not string
    auto exval = ss::json_value(42);
    slassert(ss::json_type::integer == exval.type());
    bool caught = throws_exc([&exval] { exval.as_string_or_throw(); });
    slassert(caught);
    // setters
    slassert(rv.set_string("foo"));
    slassert("foo" == rv.as_string());
    slassert(!rv.set_bool(true));
    slassert(!rv.set_int64(42));
    slassert(!rv.set_double(42.0));
    // copy
    const std::string str{"43"};
    ss::json_value rvc{str};
    slassert("43" == rvc.as_string());       
}

void test_string_default() {
    ss::json_value rv{};
    (void) rv;
    slassert("42" == rv.as_string("42"));
}

void test_int() {
    ss::json_value rv{42};
    slassert(ss::json_type::integer == rv.type());
    slassert(0 == rv.as_object().size());
    slassert(0 == rv.as_array().size());
    slassert(0 == rv.as_string().length());
    slassert(42 == rv.as_int64());
    slassert(-1 < rv.as_double() && rv.as_double() < 1);
    slassert(!rv.as_bool());
    // setters
    slassert(rv.set_int64(43));
    slassert(43 == rv.as_int64());
    slassert(!rv.set_bool(true));
    slassert(!rv.set_double(42.0));
    slassert(!rv.set_string("foo"));
    // limits
    // int64
    auto st = ss::json_value("foo");
    slassert(throws_exc([&st] { st.as_int64_or_throw(); }))
    rv.set_int64(std::numeric_limits<int64_t>::max());
    slassert(!throws_exc([&rv] { rv.as_int64_or_throw(); }))
    // int32
    rv.set_int64(std::numeric_limits<int32_t>::max());
    slassert(!throws_exc([&rv] { rv.as_int32_or_throw(); }))
    rv.set_int64(static_cast<int64_t>(std::numeric_limits<int32_t>::max()) + 1);
    slassert(throws_exc([&rv] { rv.as_int32_or_throw(); }))
    // uint32
    rv.set_int64(std::numeric_limits<uint32_t>::max());
    slassert(!throws_exc([&rv] { rv.as_uint32_or_throw(); }))
    rv.set_int64(static_cast<int64_t> (std::numeric_limits<uint32_t>::max()) + 1);
    slassert(throws_exc([&rv] { rv.as_uint32_or_throw(); }))
    rv.set_int64(std::numeric_limits<uint32_t>::max());
    slassert(!throws_exc([&rv] { rv.as_uint32_positive_or_throw(); }))
    rv.set_int64(-1);
    slassert(throws_exc([&rv] { rv.as_uint32_positive_or_throw(); }))
    rv.set_int64(0);
    slassert(throws_exc([&rv] { rv.as_uint32_positive_or_throw(); }))
    // int16
    rv.set_int64(std::numeric_limits<int16_t>::max());
    slassert(!throws_exc([&rv] { rv.as_int16_or_throw(); }))
    rv.set_int64(static_cast<int64_t> (std::numeric_limits<int16_t>::max()) + 1);
    slassert(throws_exc([&rv] { rv.as_int16_or_throw(); }))
    rv.set_int64(std::numeric_limits<int16_t>::max());
    slassert(!throws_exc([&rv] { rv.as_uint16_positive_or_throw(); }))
    rv.set_int64(-1);
    slassert(throws_exc([&rv] { rv.as_uint16_positive_or_throw(); }))
    rv.set_int64(0);
    slassert(throws_exc([&rv] { rv.as_uint16_positive_or_throw(); }))
    // uint16
    rv.set_int64(std::numeric_limits<uint16_t>::max());
    slassert(!throws_exc([&rv] { rv.as_uint16_or_throw(); }))
    rv.set_int64(static_cast<int64_t> (std::numeric_limits<uint16_t>::max()) + 1);
    slassert(throws_exc([&rv] { rv.as_uint16_or_throw(); }))
}

void test_int_default() {
    ss::json_value rv{};
    slassert(42 == rv.as_int64(42));
}

void test_real() {
    ss::json_value rv{42.0};
    slassert(ss::json_type::real == rv.type());
    slassert(0 == rv.as_object().size());
    slassert(0 == rv.as_array().size());
    slassert(0 == rv.as_string().length());
    slassert(0 == rv.as_int64());
    slassert(41 < rv.as_double() && rv.as_double() < 43);
    slassert(41 < rv.as_float() && rv.as_float() < 43);    
    slassert(!rv.as_bool());
    // setters
    slassert(rv.set_double(43.0));
    slassert(42 < rv.as_double() && rv.as_double() < 44);
    slassert(rv.set_float(43.0));
    slassert(42 < rv.as_float() && rv.as_float() < 44);
    slassert(!rv.set_bool(true));
    slassert(!rv.set_int64(42));
    slassert(!rv.set_string("foo"));
    // limits
    auto st = ss::json_value("foo");
    slassert(throws_exc([&st] { st.as_double_or_throw(); }))
    rv.set_double(std::numeric_limits<double>::max());
    slassert(!throws_exc([&rv] { rv.as_double_or_throw(); }))
    // float
    rv.set_double(std::numeric_limits<float>::max());
    slassert(!throws_exc([&rv] { rv.as_float_or_throw(); }))
    rv.set_double(static_cast<double>(std::numeric_limits<float>::max()) * 2);
    slassert(throws_exc([&rv] { rv.as_float_or_throw(); }))
}

void test_real_default() {
    ss::json_value rv{};
    slassert(41 < rv.as_double(42) && rv.as_double(42) < 43);
}

void test_boolean() {
    ss::json_value rvt{true};
    slassert(ss::json_type::boolean == rvt.type());
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

    ss::json_value rvf{false};
    slassert(ss::json_type::boolean == rvf.type());
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
    
    // throw
    auto st = ss::json_value("foo");
    slassert(throws_exc([&st] { st.as_bool_or_throw(); }))
}

void test_boolean_default() {
    ss::json_value rv{};
    slassert(rv.as_bool(true));
    slassert(!rv.as_bool(false));
}

void test_field_by_name() {
    TestRefl tr{};
    auto rv = tr.get_reflected_value();
    
    auto& rvf = rv["transient_f4"];
    slassert(ss::json_type::array == rvf.type());
    slassert(2 == rvf.as_array().size());
    slassert(42 == rvf.as_array()[0].as_int64());
    slassert("foo" == rvf.as_array()[1].as_string());
    
    auto& rv_null = rv["aaa"];
    slassert(ss::json_type::nullt == rv_null.type());
}

#ifdef STATICLIB_WITH_ICU
void test_icu() {
    icu::UnicodeString st{"foo"};
    ss::json_value val{st};
    slassert(icu::UnicodeString{"foo"} == val.as_ustring());
    val.set_ustring("bar");
    slassert(icu::UnicodeString{"bar"} == val.as_ustring());
}
#endif // STATICLIB_WITH_ICU

void test_get_or_throw() {
    TestRefl tr{};
    auto rv = tr.get_reflected_value();
    
    // get existing
    slassert(41 == rv["f1"].as_int32());
    slassert(41 == rv.getattr("f1").as_int32());
    slassert(41 == rv.getattr_or_throw("f1").as_int32());
#ifdef STATICLIB_WITH_ICU
    slassert(41 == rv.getattru("f1").as_int32());
    slassert(41 == rv.getattru_or_throw("f1").as_int32());
#endif // STATICLIB_WITH_ICU
    
    // create new attr
    ss::json_value& nval = rv.getattr_or_throw("foo");
    slassert(ss::json_type::nullt == nval.type());
    nval.set_int32(42);
    slassert(ss::json_type::integer == nval.type());
    nval.set_string("foo");
    slassert(ss::json_type::string == nval.type());
    nval.set_float(static_cast<float>(0.1));
    slassert(ss::json_type::real == nval.type());
    nval.set_bool(false);
    slassert(ss::json_type::boolean == nval.type());
    std::vector<ss::json_value> arrval{};
    arrval.emplace_back("bar");
    nval.set_array(std::move(arrval));
    slassert(ss::json_type::array == nval.type());
    slassert("bar" == nval.as_array()[0].as_string());
    std::vector<ss::json_field> objval{};
    objval.emplace_back("baz", 42);
    nval.set_object(std::move(objval));
    slassert(ss::json_type::object == nval.type());
    slassert("baz" == nval.as_object()[0].name());
    slassert(42 == nval.as_object()[0].value().as_int32());
    
    // not object
    ss::json_value& exval = rv.getattr_or_throw("f1");
    slassert(ss::json_type::integer == exval.type());
    bool caught = throws_exc([&exval] { exval.getattr_or_throw("foo"); });
    slassert(caught);
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
        test_field_by_name();
#ifdef STATICLIB_WITH_ICU
        test_icu();
#endif // STATICLIB_WITH_ICU
        test_get_or_throw();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
