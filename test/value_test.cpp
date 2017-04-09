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

#include "staticlib/json/value.hpp"

#include <iostream>
#include <functional>
#include <limits>

#include "staticlib/config/assert.hpp"

#include "staticlib/json/field.hpp"

bool throws_exc(std::function<void()> fun) {
    try {
        fun();
    } catch (const sl::json::json_exception& e) {
        (void) e;
        return true;
    }
    return false;
}

class test_refl {
    int32_t f1 = 41;
    std::string f2 = "42";
    bool f3 = true;

public:
    sl::json::value get_reflected() const {
        return {
            {"f1", f1},
            {"f2", f2},
            {"f3", f3},
            {"transient_f4", [] {
                std::vector<sl::json::value> vec;
                vec.emplace_back(42);
                vec.emplace_back("foo");
                return vec;
            }()},
            {"transient_f5", {
                {"key1", "val1"},
                {"key2", "val2"}
            }},
            {"fnullable", nullptr}
        };
    }
};

void test_null() { 
    sl::json::value rv{};
    slassert(sl::json::type::nullt == rv.json_type());
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
    sl::json::value rv = test_refl{}.get_reflected().clone();
    slassert(sl::json::type::object == rv.json_type());
    slassert(6 == rv.as_object().size());
    slassert("f1" == rv.as_object()[0].name());
    slassert(41 == rv.as_object()[0].val().as_int64());
    slassert("f2" == rv.as_object()[1].name());
    slassert("42" == rv.as_object()[1].as_string());
    slassert(sl::json::type::integer == rv.as_object()[0].val().json_type());
    slassert(sl::json::type::string == rv.as_object()[1].val().json_type());
    slassert(sl::json::type::boolean == rv.as_object()[2].val().json_type());
    slassert(sl::json::type::array == rv.as_object()[3].val().json_type());
    slassert(sl::json::type::object == rv.as_object()[4].val().json_type());    
    slassert(sl::json::type::nullt == rv.as_object()[5].val().json_type());
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
    sl::json::value& exval = rv.getattr_or_throw("f1");
    slassert(sl::json::type::integer == exval.json_type());
    bool caught = throws_exc([&exval]{ exval.as_object_or_throw(); });
    slassert(caught);
    // setters
    slassert(!rv.set_bool(true));
    slassert(!rv.set_int64(42));
    slassert(!rv.set_double(42.0));
    slassert(!rv.set_string("foo"));
}

void test_array() {
    std::vector<sl::json::value> vec{};
    vec.emplace_back(42);
    vec.emplace_back(true);
    sl::json::value rv{std::move(vec)};
    slassert(sl::json::type::array == rv.json_type());
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
    auto exval = sl::json::value(42);
    slassert(sl::json::type::integer == exval.json_type());
    bool caught = throws_exc([&exval] { exval.as_array_or_throw(); });
    slassert(caught);
    // setters
    slassert(!rv.set_bool(true));
    slassert(!rv.set_int64(42));
    slassert(!rv.set_double(42.0));
    slassert(!rv.set_string("foo"));
}

void test_string() {
    sl::json::value rv{"42"};
    slassert(sl::json::type::string == rv.json_type());
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
    slassert("43" == rv.as_string_nonempty_or_throw());
    // not string
    auto exval = sl::json::value(42);
    slassert(sl::json::type::integer == exval.json_type());
    bool caught = throws_exc([&exval] { exval.as_string_or_throw(); });
    slassert(caught);
    // setters
    slassert(rv.set_string("foo"));
    slassert("foo" == rv.as_string());
    slassert(!rv.set_bool(true));
    slassert(!rv.set_int64(42));
    slassert(!rv.set_double(42.0));
    // empty
    slassert(!rv.set_string(""));
    bool caught_empty = throws_exc([&rv] { rv.as_string_nonempty_or_throw(); });
    slassert(caught_empty);
    // copy
    const std::string str{"43"};
    sl::json::value rvc{str};
    slassert("43" == rvc.as_string());       
}

void test_string_default() {
    sl::json::value rv{};
    (void) rv;
    slassert("42" == rv.as_string("42"));
}

void test_int() {
    sl::json::value rv{42};
    slassert(sl::json::type::integer == rv.json_type());
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
    auto st = sl::json::value("foo");
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
    sl::json::value rv{};
    slassert(42 == rv.as_int64(42));
}

void test_real() {
    sl::json::value rv{42.0};
    slassert(sl::json::type::real == rv.json_type());
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
    auto st = sl::json::value("foo");
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
    sl::json::value rv{};
    slassert(41 < rv.as_double(42) && rv.as_double(42) < 43);
}

void test_boolean() {
    sl::json::value rvt{true};
    slassert(sl::json::type::boolean == rvt.json_type());
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

    sl::json::value rvf{false};
    slassert(sl::json::type::boolean == rvf.json_type());
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
    auto st = sl::json::value("foo");
    slassert(throws_exc([&st] { st.as_bool_or_throw(); }))
}

void test_boolean_default() {
    sl::json::value rv{};
    slassert(rv.as_bool(true));
    slassert(!rv.as_bool(false));
}

void test_field_by_name() {
    test_refl tr{};
    auto rv = tr.get_reflected();
    
    auto& rvf = rv["transient_f4"];
    slassert(sl::json::type::array == rvf.json_type());
    slassert(2 == rvf.as_array().size());
    slassert(42 == rvf.as_array()[0].as_int64());
    slassert("foo" == rvf.as_array()[1].as_string());
    
    auto& rv_null = rv["aaa"];
    slassert(sl::json::type::nullt == rv_null.json_type());
}

void test_get_or_throw() {
    test_refl tr{};
    auto rv = tr.get_reflected();
    
    // get existing
    slassert(41 == rv["f1"].as_int32());
    slassert(41 == rv.getattr("f1").as_int32());
    slassert(41 == rv.getattr_or_throw("f1").as_int32());
    
    // create new attr
    sl::json::value& nval = rv.getattr_or_throw("foo");
    slassert(sl::json::type::nullt == nval.json_type());
    nval.set_int32(42);
    slassert(sl::json::type::integer == nval.json_type());
    nval.set_string("foo");
    slassert(sl::json::type::string == nval.json_type());
    nval.set_float(static_cast<float>(0.1));
    slassert(sl::json::type::real == nval.json_type());
    nval.set_bool(false);
    slassert(sl::json::type::boolean == nval.json_type());
    std::vector<sl::json::value> arrval{};
    arrval.emplace_back("bar");
    nval.set_array(std::move(arrval));
    slassert(sl::json::type::array == nval.json_type());
    slassert("bar" == nval.as_array()[0].as_string());
    std::vector<sl::json::field> objval{};
    objval.emplace_back("baz", 42);
    nval.set_object(std::move(objval));
    slassert(sl::json::type::object == nval.json_type());
    slassert("baz" == nval.as_object()[0].name());
    slassert(42 == nval.as_object()[0].val().as_int32());
    
    // not object
    sl::json::value& exval = rv.getattr_or_throw("f1");
    slassert(sl::json::type::integer == exval.json_type());
    bool caught = throws_exc([&exval] { exval.getattr_or_throw("foo"); });
    slassert(caught);
}

void test_tmp() {
    auto val = sl::json::value("42");
    std::cout << sl::json::stringify_json_type(val.json_type()) << std::endl;
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
        test_get_or_throw();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
