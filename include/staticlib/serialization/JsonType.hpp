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
 * File:   JsonType.hpp
 * Author: alex
 *
 * Created on December 28, 2014, 8:21 PM
 */

#ifndef STATICLIB_SERIALIZATION_JSONTYPE_HPP
#define	STATICLIB_SERIALIZATION_JSONTYPE_HPP

#include <string>
#include <cstdint>

namespace staticlib {
namespace serialization {

/**
 * @enum JsonType
 * Enumeration with all possible types of reflected values.
 * Matches types supported by Jansson JSON library.
 */
enum class JsonType : uint8_t {
    NULL_T,
    OBJECT,
    ARRAY,
    STRING,
    INTEGER,
    REAL,
    BOOLEAN
};

/**
 * Helper standalone function that converts `JsonType` values into string representation.
 * 
 * @param jt type enumeration value
 * @return string representation of the specified value
 */
std::string stringify_json_type(JsonType jt);

}
} // namespace

#endif	/* STATICLIB_SERIALIZATION_JSONTYPE_HPP */

