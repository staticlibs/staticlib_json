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

#ifndef STATICLIB_SERIALIZATION_JSON_HPP
#define	STATICLIB_SERIALIZATION_JSON_HPP

#include <streambuf>
#include <string>
#include <functional>

#ifdef STATICLIB_WITH_ICU
#include <unicode/unistr.h>
#endif // STATICLIB_WITH_ICU

#include "staticlib/io.hpp"

#include "staticlib/serialization/JsonField.hpp"
#include "staticlib/serialization/JsonValue.hpp"

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
 * Serializes specified reference to JSON. 
 * JSON is written to the specified sink. Preserves order of object fields.
 * 
 * @param value reference to reflected value instance
 * @param dest sink to write JSON into
 * @return JSON string
 * @throws SerializationException
 */
template <typename Sink>
void dump_json(const JsonValue& value, Sink& dest) {
    auto sbuf = staticlib::io::make_unbuffered_ostreambuf(std::ref(dest));
    dump_json_to_streambuf(value, sbuf);
}

/**
 * Serializes specified reference (usually obtained from 'Json' instance)
 * to JSON. JSON is written to the specified streambuf. Preserves order of object fields.
 * 
 * @param value reference to reflected value instance
 * @param dest streambuf to write JSON into
 * @return JSON string
 * @throws SerializationException
 */
void dump_json_to_streambuf(const JsonValue& value, std::streambuf& dest);
    
/**
 * Serializes specified reference (usually obtained from 'Json' instance)
 * to JSON string. Preserves order of object fields.
 * 
 * @param value reference to reflected value instance
 * @return JSON string
 * @throws SerializationException
 */
std::string dump_json_to_string(const JsonValue& value);
    
#ifdef STATICLIB_WITH_ICU
/**
 * Serializes specified reference (usually obtained from 'Json' instance)
 * to JSON string. Preserves order of object fields.
 * 
 * @param value reference to reflected value instance
 * @return JSON string
 * @throws SerializationException
 */
icu::UnicodeString dump_json_to_ustring(const JsonValue& value);
#endif // STATICLIB_WITH_ICU

/**
 * Deserializes data from specified source into 'JsonValue'.
 * Supports 'bare' (non-object, non-array) JSON input.
 * Supports partial input: will read only first valid JSON element 
 * from input source.
 * 
 * @param src source with JSON
 * @return instance of 'JsonValue'
 * @throws SerializarionException      
 */
template <typename Source>
JsonValue load_json(Source& src) {
    auto sbuf = staticlib::io::make_unbuffered_istreambuf(std::ref(src));
    return load_json_from_streambuf(sbuf);
}

/**
 * Deserializes data from specified streambuf into 'JsonValue'.
 * Supports 'bare' (non-object, non-array) JSON input.
 * Supports partial input: will read only first valid JSON element 
 * from input string.
 * 
 * @param src streambuf with JSON
 * @return instance of 'JsonValue'
 * @throws SerializarionException      
 */
JsonValue load_json_from_streambuf(std::streambuf& src);

/**
 * Deserializes specified string into 'JsonValue'.
 * Supports 'bare' (non-object, non-array) JSON input.
 * Supports partial input: will read only first valid JSON element 
 * from input string.
 * 
 * @param str JSON string
 * @return instance of 'JsonValue'
 * @throws SerializarionException      
 */
JsonValue load_json_from_string(const std::string& str);
    
#ifdef STATICLIB_WITH_ICU
/**
 * Deserializes specified string into 'JsonValue'.
 * Supports 'bare' (non-object, non-array) JSON input.
 * Supports partial input: will read only first valid JSON element 
 * from input string.
 * 
 * @param str JSON string
 * @return instance of 'JsonValue'
 * @throws SerializarionException      
 */
JsonValue load_json_from_ustring(const icu::UnicodeString& str);
#endif // STATICLIB_WITH_ICU

} // namespace
} 

#endif	/* STATICLIB_SERIALIZATION_JSON_HPP */

