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
 * File:   jansson_deleter.hpp
 * Author: alex
 *
 * Created on April 9, 2017, 8:28 PM
 */

#ifndef STATICLIB_JSON_JANSSON_DELETER_HPP
#define	STATICLIB_JSON_JANSSON_DELETER_HPP

namespace staticlib {
namespace json {

class jansson_deleter {
public:
    void operator()(json_t* json) {
        json_decref(json);
    }
};

} // namespace
}

#endif	/* STATICLIB_JSON_JANSSON_DELETER_HPP */
