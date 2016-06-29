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
 * File:   JsonField_test.cpp
 * Author: alex
 *
 * Created on June 29, 2016, 9:47 AM
 */

#include "staticlib/serialization/JsonField.hpp"

#include <iostream>

#include "staticlib/config/assert.hpp"

namespace ss = staticlib::serialization;

void test_string() {
    ss::JsonField fi{"foo", "bar"};
    slassert("foo" == fi.get_name());
    slassert("bar" == fi.get_value().get_string());
}

#ifdef STATICLIB_WITH_ICU
void test_icu() {
    ss::JsonField fi{icu::UnicodeString{"foo"}, icu::UnicodeString{"bar"}};
    slassert("foo" == fi.get_name());
    slassert(icu::UnicodeString{"foo"} == fi.get_uname());
    slassert("bar" == fi.get_value().get_string());
    slassert(icu::UnicodeString{"bar"} == fi.get_value().get_ustring());
}
#endif // STATICLIB_WITH_ICU

int main() {
    try {
        test_string();
#ifdef STATICLIB_WITH_ICU        
        test_icu();
#endif // STATICLIB_WITH_ICU        
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}

