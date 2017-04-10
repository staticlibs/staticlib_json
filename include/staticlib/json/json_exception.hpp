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

