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
 * File:   json_type.cpp
 * Author: alex
 * 
 * Created on January 23, 2015, 9:52 PM
 */

#include "staticlib/serialization/json_type.hpp"

namespace staticlib {
namespace serialization {

std::string stringify_json_type(json_type jt) { 
    switch(jt) {
    case json_type::nullt: return "NULL_T";
    case json_type::object: return "OBJECT";
    case json_type::array: return "ARRAY";
    case json_type::string: return "STRING";
    case json_type::integer: return "INTEGER";
    case json_type::real: return "REAL";
    case json_type::boolean: return "BOOLEAN";
    default: return "UNKNOWN";
    }
}

} // namespace
} 
