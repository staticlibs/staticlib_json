/* 
 * File:   SerializationException.hpp
 * Author: alex
 *
 * Created on January 2, 2015, 2:22 PM
 */

#ifndef STATICLIB_SERIALIZATIONEXCEPTION_HPP
#define	STATICLIB_SERIALIZATIONEXCEPTION_HPP

#include "staticlib/utils/BaseException.hpp"

namespace staticlib {
namespace serialization {

/**
 * Module specific exception
 */
class SerializationException : public staticlib::utils::BaseException {
public:
    /**
     * Default constructor
     */
    SerializationException() = default;

    /**
     * Constructor with message
     * 
     * @param msg error message
     */
    SerializationException(const std::string& msg) :
    staticlib::utils::BaseException(msg) { }
};

}
} // namespace

#endif	/* STATICLIB_SERIALIZATIONEXCEPTION_HPP */

