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
 * File:   JsonType.cpp
 * Author: alex
 * 
 * Created on January 23, 2015, 9:52 PM
 */

#include "staticlib/serialization/JsonType.hpp"

namespace staticlib {
namespace serialization {

std::string stringify_json_type(JsonType jt) { 
    switch(jt) {
    case JsonType::NULL_T: return "NULL_T";
    case JsonType::OBJECT: return "OBJECT";
    case JsonType::ARRAY: return "ARRAY";
    case JsonType::STRING: return "STRING";
    case JsonType::INTEGER: return "INTEGER";
    case JsonType::REAL: return "REAL";
    case JsonType::BOOLEAN: return "BOOLEAN";
    default: return "UNKNOWN";
    }
}

} // namespace
} 
