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
 * File:   json_field_test.cpp
 * Author: alex
 *
 * Created on June 29, 2016, 9:47 AM
 */

#include "staticlib/json/field.hpp"

#include <iostream>

#include "staticlib/config/assert.hpp"

void test_string() {
    sl::json::field fi{"foo", "bar"};
    slassert("foo" == fi.name());
    slassert("bar" == fi.val().as_string());
}

int main() {
    try {
        test_string();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

