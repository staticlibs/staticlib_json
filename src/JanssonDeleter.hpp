/* 
 * File:   JanssonDeleter.hpp
 * Author: alex
 *
 * Created on January 2, 2015, 11:05 AM
 */

#ifndef STATICLIB_JANSSONDELETER_HPP
#define	STATICLIB_JANSSONDELETER_HPP

#include "jansson.h"

namespace staticlib {
namespace serialization {

class JanssonDeleter {
public:
    void operator()(json_t* json) {
        json_decref(json);
    }
};

}
} // namespace

#endif	/* STATICLIB_JANSSONDELETER_HPP */

