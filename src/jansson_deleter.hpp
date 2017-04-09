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

