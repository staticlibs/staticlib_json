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
 * File:   json.hpp
 * Author: alex
 *
 * Created on January 1, 2015, 6:11 PM
 */

#ifndef STATICLIB_JSON_HPP
#define	STATICLIB_JSON_HPP

#include <string>
#include <streambuf>

#ifdef STATICLIB_WITH_ICU
#include <unicode/unistr.h>
#endif // STATICLIB_WITH_ICU

#include "staticlib/reflection.hpp"

namespace staticlib {
namespace serialization {

    /**
     * Initializes Jansson hash function, not thread-safe, can be called before
     * first JSON operations, not required in single-thread environment
     * 
     * @throws nothing
     */
    void init();

    /**
     * Serializes specified reference (usually obtained from 'Reflected' instance)
     * to JSON. JSON is written to the specified streambuf. Preserves order of object fields.
     * 
     * @param value reference to reflected value instance
     * @param dest streambuf to write JSON into
     * @return JSON string
     * @throws SerializationException
     */
    void dump_json(const staticlib::reflection::ReflectedValue& value, std::streambuf& dest);
    
    /**
     * Serializes specified reference (usually obtained from 'Reflected' instance)
     * to JSON string. Preserves order of object fields.
     * 
     * @param value reference to reflected value instance
     * @return JSON string
     * @throws SerializationException
     */
    std::string dump_json_to_string(const staticlib::reflection::ReflectedValue& value);
    
#ifdef STATICLIB_WITH_ICU
    /**
     * Serializes specified reference (usually obtained from 'Reflected' instance)
     * to JSON string. Preserves order of object fields.
     * 
     * @param value reference to reflected value instance
     * @return JSON string
     * @throws SerializationException
     */
    icu::UnicodeString dump_json_to_ustring(const staticlib::reflection::ReflectedValue& value);
#endif // STATICLIB_WITH_ICU
    
    /**
     * Deserializes data from specified streambuf into 'ReflectedValue'.
     * Supports 'bare' (non-object, non-array) JSON input.
     * Supports partial input: will read only first valid JSON element 
     * from input string.
     * 
     * @param src streambuf with JSON
     * @return instance of 'ReflectedValue'
     * @throws SerializarionException      
     */
    staticlib::reflection::ReflectedValue load_json(std::streambuf& src);
    
    /**
     * Deserializes specified string into 'ReflectedValue'.
     * Supports 'bare' (non-object, non-array) JSON input.
     * Supports partial input: will read only first valid JSON element 
     * from input string.
     * 
     * @param str JSON string
     * @return instance of 'ReflectedValue'
     * @throws SerializarionException      
     */
    staticlib::reflection::ReflectedValue load_json_from_string(const std::string& str);
    
#ifdef STATICLIB_WITH_ICU
    /**
     * Deserializes specified string into 'ReflectedValue'.
     * Supports 'bare' (non-object, non-array) JSON input.
     * Supports partial input: will read only first valid JSON element 
     * from input string.
     * 
     * @param str JSON string
     * @return instance of 'ReflectedValue'
     * @throws SerializarionException      
     */
    staticlib::reflection::ReflectedValue load_json_from_ustring(const icu::UnicodeString& str);
#endif // STATICLIB_WITH_ICU

}
} // namespace

#endif	/* STATICLIB_JSON_HPP */

