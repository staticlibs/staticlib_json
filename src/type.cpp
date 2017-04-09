/* 
 * File:   type.cpp
 * Author: alex
 *
 * Created on April 9, 2017, 5:48 PM
 */

#include "staticlib/json/type.hpp"

namespace staticlib {
namespace json {

std::string stringify_json_type(type jt) {
    switch (jt) {
    case type::nullt: return "json::type::nullt";
    case type::object: return "json::type::object";
    case type::array: return "json::type::array";
    case type::string: return "json::type::string";
    case type::integer: return "json::type::integer";
    case type::real: return "json::type::real";
    case type::boolean: return "json::type::boolean";
    default: return "unknown";
    }
}

} // namespace
} 

