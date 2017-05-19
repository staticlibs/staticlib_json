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
 * File:   operations.cpp
 * Author: alex
 *
 * Created on April 9, 2017, 8:42 PM
 */

#include "staticlib/json/operations.hpp"

#include "jansson_ops.hpp"

namespace staticlib {
namespace json {

void init() {
    jansson_init();
}

value load(std::streambuf* src) {
    return jansson_load_from_streambuf(src);
}

value loads(const std::string& str) {
    return jansson_load_from_string(str);
}

std::string dumps(const value& json) {
    return json.dumps();
}

const value& null_value_ref() {
    static value empty;
    return empty;
}

} // namespace
}

