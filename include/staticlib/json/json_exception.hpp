/* 
 * File:   json_exception.hpp
 * Author: alex
 *
 * Created on April 9, 2017, 11:59 AM
 */

#ifndef STATICLIB_JSON_JSON_EXCEPTION_HPP
#define	STATICLIB_JSON_JSON_EXCEPTION_HPP

#include "staticlib/support.hpp"

namespace staticlib {
namespace json {

/**
 * Module specific exception
 */
class json_exception : public staticlib::support::exception {
public:
    /**
     * Default constructor
     */
    json_exception() = default;

    /**
     * Constructor with message
     * 
     * @param msg error message
     */
    json_exception(const std::string& msg) :
    staticlib::support::exception(msg) { }
};

}
} // namespace


#endif	/* STATICLIB_JSON_JSON_EXCEPTION_HPP */

