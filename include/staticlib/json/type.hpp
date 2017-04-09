/* 
 * File:   type.hpp
 * Author: alex
 *
 * Created on April 9, 2017, 11:58 AM
 */

#ifndef STATICLIB_JSON_TYPE_HPP
#define	STATICLIB_JSON_TYPE_HPP

#include <cstdint>
#include <string>

namespace staticlib {
namespace json {

/**
 * @enum type
 * Enumeration with all possible types of values.
 * Matches types supported by Jansson JSON library.
 */
enum class type {
    nullt,
    object,
    array,
    string,
    integer,
    real,
    boolean
};

/**
 * Helper standalone function that converts `json::type` values into string representation.
 * 
 * @param jt type enumeration value
 * @return string representation of the specified value
 */
std::string stringify_json_type(type jt);

} // namespace
}


#endif	/* STATICLIB_JSON_TYPE_HPP */

