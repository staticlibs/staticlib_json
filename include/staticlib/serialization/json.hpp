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

#include <memory>

#ifdef STATICLIB_WITH_ICU
#include <unicode/unistr.h>
#else
#include <string>
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
     * to JSON string. Preserves order of object fields.
     * 
     * @param value reference to reflected value instance
     * @return JSON string
     * @throws SerializationException
     */
#ifdef STATICLIB_WITH_ICU
    icu::UnicodeString dumps_json(const staticlib::reflection::ReflectedValue& value);
#else
    std::string dumps_json(const staticlib::reflection::ReflectedValue& value);
#endif // STATICLIB_WITH_ICU
    
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
#ifdef STATICLIB_WITH_ICU
    staticlib::reflection::ReflectedValue loads_json(const icu::UnicodeString& str);
#else    
    staticlib::reflection::ReflectedValue loads_json(const std::string& str);
#endif // STATICLIB_WITH_ICU

}
} // namespace

#endif	/* STATICLIB_JSON_HPP */

