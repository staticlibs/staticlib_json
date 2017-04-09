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
static value load(Source& src) {
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
 * Reference to null json value
 * 
 * @return null json value reference
 */
const value& null_value_ref();

} // namespace
}

#endif	/* STATICLIB_JSON_OPERATIONS_HPP */

