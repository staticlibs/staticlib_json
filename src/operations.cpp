/* 
 * File:   operations.cpp
 * Author: alex
 *
 * Created on April 9, 2017, 8:42 PM
 */

#include "staticlib/json/operations.hpp"

#include "jansson_ops.hpp"

namespace staticlib {
namespace json {

void init() {
    jansson_init();
}

value load(std::streambuf* src) {
    return jansson_load_from_streambuf(src);
}

value loads(const std::string& str) {
    return jansson_load_from_string(str);
}

const value& null_value_ref() {
    static value empty;
    return empty;
}

} // namespace
}

