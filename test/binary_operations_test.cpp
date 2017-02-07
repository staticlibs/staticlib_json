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
 * File:   binary_test.cpp
 * Author: alex
 *
 * Created on November 29, 2015, 12:30 PM
 */

#include "staticlib/serialization/binary_operations.hpp"

#include <iostream>
#include <cstdint>

#include "staticlib/config/assert.hpp"

#include "staticlib/io.hpp"
#include "staticlib/ranges.hpp"

namespace io = staticlib::io;
namespace ra = staticlib::ranges;
namespace ss = staticlib::serialization;

#pragma pack(push, 1)
struct TestStruct {
    uint16_t first;
    uint32_t second;
    
//    won't be a POD in MSVC2013
//    TestStruct() = default;
    
//    TestStruct(uint16_t first, uint32_t second):
//    first(first),
//    second(second) { }
};
#pragma pack(pop)

TestStruct create_test_struct(uint16_t first, uint32_t second) {
    TestStruct res;
    res.first = first;
    res.second = second;
    return res;
}

void test_dump() {
    std::vector<TestStruct> vec{};
    vec.emplace_back(create_test_struct(42, 43));
    vec.emplace_back(create_test_struct(44, 45));
    io::string_sink sink{};
    ss::dump_binary(vec, sink);
    slassert(12 == sink.get_string().size());
}

void test_load() {
    io::string_source src{"123456789012"};
    auto range = ss::load_binary<TestStruct>(std::move(src));
    std::vector<TestStruct> vec = ra::emplace_to_vector(std::move(range));
    slassert(2 == vec.size());
}

int main() {
    try {
        test_dump();
        test_load();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
