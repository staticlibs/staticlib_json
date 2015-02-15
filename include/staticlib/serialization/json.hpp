/* 
 * File:   json.hpp
 * Author: alex
 *
 * Created on January 1, 2015, 6:11 PM
 */

#ifndef STATICLIB_JSON_HPP
#define	STATICLIB_JSON_HPP

#include <memory>

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
    std::string dumps_json(const staticlib::reflection::ReflectedValue& value);
    
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
    staticlib::reflection::ReflectedValue loads_json(const std::string& str);

}
} // namespace

#endif	/* STATICLIB_JSON_HPP */

