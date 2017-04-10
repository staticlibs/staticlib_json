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
 * File:   type.cpp
 * Author: alex
 *
 * Created on April 9, 2017, 5:48 PM
 */

#include "staticlib/json/type.hpp"

namespace staticlib {
namespace json {

std::string stringify_json_type(type jt) {
    switch (jt) {
    case type::nullt: return "json::type::nullt";
    case type::object: return "json::type::object";
    case type::array: return "json::type::array";
    case type::string: return "json::type::string";
    case type::integer: return "json::type::integer";
    case type::real: return "json::type::real";
    case type::boolean: return "json::type::boolean";
    default: return "unknown";
    }
}

} // namespace
} 

