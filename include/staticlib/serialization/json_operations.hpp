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

#ifndef STATICLIB_SERIALIZATION_JSON_OPERATIONS_HPP
#define	STATICLIB_SERIALIZATION_JSON_OPERATIONS_HPP

#include <streambuf>
#include <string>

#ifdef STATICLIB_WITH_ICU
#include <unicode/unistr.h>
#endif // STATICLIB_WITH_ICU

#include "staticlib/io.hpp"

#include "staticlib/serialization/json_field.hpp"
#include "staticlib/serialization/json_value.hpp"

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
 * Serializes specified reference (usually obtained from 'Json' instance)
 * to JSON. JSON is written to the specified streambuf. Preserves order of object fields.
 * 
 * @param value reference to reflected value instance
 * @param dest streambuf to write JSON into
 * @return JSON string
 * @throws serialization_exception
 */
void dump_json_to_streambuf(const json_value& value, std::streambuf& dest);

/**
 * Serializes specified reference to JSON. 
 * JSON is written to the specified sink. Preserves order of object fields.
 * 
 * @param value reference to reflected value instance
 * @param dest sink to write JSON into
 * @return JSON string
 * @throws serialization_exception
 */
template <typename Sink>
void dump_json(const json_value& value, Sink& dest) {
    auto sbuf = staticlib::io::make_unbuffered_ostreambuf(io::make_reference_sink(dest));
    dump_json_to_streambuf(value, sbuf);
}


/**
 * Serializes specified reference (usually obtained from 'Json' instance)
 * to JSON string. Preserves order of object fields.
 * 
 * @param value reference to reflected value instance
 * @return JSON string
 * @throws serialization_exception
 */
std::string dump_json_to_string(const json_value& value);
    
#ifdef STATICLIB_WITH_ICU
/**
 * Serializes specified reference (usually obtained from 'Json' instance)
 * to JSON string. Preserves order of object fields.
 * 
 * @param value reference to reflected value instance
 * @return JSON string
 * @throws serialization_exception
 */
icu::UnicodeString dump_json_to_ustring(const json_value& value);
#endif // STATICLIB_WITH_ICU

/**
 * Deserializes data from specified streambuf into 'json_value'.
 * Supports 'bare' (non-object, non-array) JSON input.
 * Supports partial input: will read only first valid JSON element 
 * from input string.
 * 
 * @param src streambuf with JSON
 * @return instance of 'json_value'
 * @throws SerializarionException      
 */
json_value load_json_from_streambuf(std::streambuf& src);

/**
 * Deserializes data from specified source into 'json_value'.
 * Supports 'bare' (non-object, non-array) JSON input.
 * Supports partial input: will read only first valid JSON element 
 * from input source.
 * 
 * @param src source with JSON
 * @return instance of 'json_value'
 * @throws SerializarionException      
 */
template <typename Source>
json_value load_json(Source& src) {
    auto sbuf = staticlib::io::make_unbuffered_istreambuf(io::make_reference_source(src));
    return load_json_from_streambuf(sbuf);
}

/**
 * Deserializes specified string into 'json_value'.
 * Supports 'bare' (non-object, non-array) JSON input.
 * Supports partial input: will read only first valid JSON element 
 * from input string.
 * 
 * @param str JSON string
 * @return instance of 'json_value'
 * @throws SerializarionException      
 */
json_value load_json_from_string(const std::string& str);
    
#ifdef STATICLIB_WITH_ICU
/**
 * Deserializes specified string into 'json_value'.
 * Supports 'bare' (non-object, non-array) JSON input.
 * Supports partial input: will read only first valid JSON element 
 * from input string.
 * 
 * @param str JSON string
 * @return instance of 'json_value'
 * @throws SerializarionException      
 */
json_value load_json_from_ustring(const icu::UnicodeString& str);
#endif // STATICLIB_WITH_ICU

} // namespace
} 

#endif	/* STATICLIB_SERIALIZATION_JSON_OPERATIONS_HPP */

