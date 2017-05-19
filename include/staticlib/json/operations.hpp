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
 * File:   operations.hpp
 * Author: alex
 *
 * Created on April 9, 2017, 8:41 PM
 */

#ifndef STATICLIB_JSON_OPERATIONS_HPP
#define	STATICLIB_JSON_OPERATIONS_HPP

#include <streambuf>
#include <string>

#include "staticlib/json/field.hpp"
#include "staticlib/json/value.hpp"
#include "staticlib/json/json_exception.hpp"

namespace staticlib {
namespace json {

/**
 * Initializes Jansson hash function, not thread-safe, can be called before
 * first JSON operations, not required in single-thread environment
 * 
 * @throws nothing
 */
void init();

/**
 * Deserializes data from specified streambuf into 'json::value'.
 * Supports 'bare' (non-object, non-array) JSON input.
 * Supports partial input: will read only first valid JSON element 
 * from input string.
 * 
 * @param src streambuf with JSON
 * @return instance of 'json::value'
 * @throws json_exception      
 */
value load(std::streambuf* src);

/**
 * Deserializes data from specified source into 'json::value'.
 * Supports 'bare' (non-object, non-array) JSON input.
 * Supports partial input: will read only first valid JSON element 
 * from input source.
 * 
 * @param src source with JSON
 * @return instance of 'json::value'
 * @throws json_exception      
 */
template <typename Source>
value load(Source& src) {
    auto sbuf = sl::io::make_unbuffered_istreambuf(src);
    return load(std::addressof(sbuf));
}

/**
 * Deserializes specified string into 'json::value'.
 * Supports 'bare' (non-object, non-array) JSON input.
 * Supports partial input: will read only first valid JSON element 
 * from input string.
 * 
 * @param str JSON string
 * @return instance of 'json::value'
 * @throws json_exception      
 */
value loads(const std::string& str);

/**
 * Shortcut function that can create 'json::value' from
 * literal and dump it to string
 * 
 * @param json json, possible literal
 * @return string representation
 */
std::string dumps(const value& json);

/**
 * Reference to null json value
 * 
 * @return null json value reference
 */
const value& null_value_ref();

} // namespace
}

#endif	/* STATICLIB_JSON_OPERATIONS_HPP */

