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

#include "staticlib/utils.hpp"
#include "staticlib/reflection.hpp"

#include "staticlib/serialization/json.hpp"

namespace { // anonymous

namespace ss = staticlib::utils;
namespace sr = staticlib::reflection;
namespace sj = staticlib::serialization;

static const std::string JSON_STR = R"({
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
    std::string f2 = "42";
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
    std::unordered_set<std::string> set{};
    for (const auto& fi : obj) {
        set.insert(fi.get_name());
        if ("f1" == fi.get_name()) {
            assert(sr::ReflectedType::INTEGER == fi.get_value().get_type());
            assert(41 == fi.get_value().get_integer());
        } else if ("f2" == fi.get_name()) {
            assert(sr::ReflectedType::STRING == fi.get_value().get_type());
            assert("42" == fi.get_value().get_string());
        } else if ("f3" == fi.get_name()) { 
            assert(sr::ReflectedType::BOOLEAN == fi.get_value().get_type());
            assert(fi.get_value().get_boolean());
        } else if ("f4" == fi.get_name()) {
            assert(sr::ReflectedType::ARRAY == fi.get_value().get_type());
            assert(2 == fi.get_value().get_array().size());
            assert(sr::ReflectedType::INTEGER == fi.get_value().get_array()[0].get_type());
            assert(41 == fi.get_value().get_array()[0].get_integer());
            assert(sr::ReflectedType::STRING == fi.get_value().get_array()[1].get_type());
            assert("43" == fi.get_value().get_array()[1].get_string());
        } else if ("f5" == fi.get_name()) {
            assert(sr::ReflectedType::OBJECT == fi.get_value().get_type());
            assert(2 == fi.get_value().get_object().size());
            assert("f42" == fi.get_value().get_object()[0].get_name());
            assert(sr::ReflectedType::INTEGER == fi.get_value().get_object()[0].get_value().get_type());
            assert(42 == fi.get_value().get_object()[0].get_value().get_integer());
            assert("fnullable" == fi.get_value().get_object()[1].get_name());
            assert(sr::ReflectedType::NULL_T == fi.get_value().get_object()[1].get_value().get_type());
        }
    }
    assert(5 == set.size());
    
}

void test_preserve_order() {
    auto vec = std::vector<sr::ReflectedField>{};
    for(auto i = 0; i < (1<<10); i++) {
        vec.emplace_back(ss::to_string(i), i);
    }
    auto rv = sr::ReflectedValue(std::move(vec));
    auto json = sj::dumps_json(rv);
    auto loaded = sj::loads_json(json);
    for (auto i = 0; i < (1<<10); i++) {
        assert(ss::to_string(i) == loaded.get_object()[i].get_name());
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

