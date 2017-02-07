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
 * File:   json_type.hpp
 * Author: alex
 *
 * Created on December 28, 2014, 8:21 PM
 */

#ifndef STATICLIB_SERIALIZATION_JSON_TYPE_HPP
#define	STATICLIB_SERIALIZATION_JSON_TYPE_HPP

#include <cstdint>
#include <string>

namespace staticlib {
namespace serialization {

/**
 * @enum json_type
 * Enumeration with all possible types of values.
 * Matches types supported by Jansson JSON library.
 */
enum class json_type : uint8_t {
    NULL_T,
    OBJECT,
    ARRAY,
    STRING,
    INTEGER,
    REAL,
    BOOLEAN
};

/**
 * Helper standalone function that converts `json_type` values into string representation.
 * 
 * @param jt type enumeration value
 * @return string representation of the specified value
 */
std::string stringify_json_type(json_type jt);

}
} // namespace

#endif	/* STATICLIB_SERIALIZATION_JSON_TYPE_HPP */

