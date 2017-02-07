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
 * File:   json_field.cpp
 * Author: alex
 * 
 * Created on January 23, 2015, 2:40 PM
 */

#include "staticlib/serialization/json_field.hpp"

#ifdef STATICLIB_WITH_ICU
#include "staticlib/icu_utils.hpp"
#endif // STATICLIB_WITH_ICU

namespace staticlib {
namespace serialization {

namespace { // anonymous

#ifdef STATICLIB_WITH_ICU
namespace su = staticlib::icu_utils;
#endif // STATICLIB_WITH_ICU

} // namespace

json_field::json_field(json_field&& other) :
jsonName(std::move(other.jsonName)), jsonValue(std::move(other.jsonValue)) { }

json_field& json_field::operator=(json_field&& other) {
    this->jsonName = std::move(other.jsonName);
    this->jsonValue = std::move(other.jsonValue);
    return *this;
}

json_field::json_field() { }

json_field::json_field(std::string name, json_value value) :
jsonName(std::move(name)), jsonValue(std::move(value)) { }

json_field::json_field(const char* name, json_value value) :
jsonName(name), jsonValue(std::move(value)) { }

#ifdef STATICLIB_WITH_ICU
json_field::json_field(icu::UnicodeString uname, json_value value) :
jsonName(su::to_utf8(uname)), jsonValue(std::move(value)) { }
#endif // STATICLIB_WITH_ICU

const std::string& json_field::name() const {
    return jsonName;
}

#ifdef STATICLIB_WITH_ICU
const icu::UnicodeString& json_field::uname() const {
    if (nullptr == jsonUname.get()) {
        jsonUname = std::unique_ptr<icu::UnicodeString>{new icu::UnicodeString{}};
        *jsonUname = icu::UnicodeString::fromUTF8(jsonName);
    }
    return *jsonUname;
}
#endif // STATICLIB_WITH_ICU

json_type json_field::type() const {
    return value().type();
}

const json_value& json_field::value() const {
    return jsonValue;
}

json_value& json_field::value() {
    return jsonValue;
}

void json_field::set_value(json_value&& value) {
    json_value previous = std::move(this->jsonValue);
    this->jsonValue = std::move(value);
}

json_field json_field::clone() const {
    return json_field(jsonName, jsonValue.clone());
}

const std::vector<json_field>& json_field::as_object() const {
    return value().as_object();
}

std::vector<json_field>& json_field::as_object_or_throw(const std::string& context) {
    return value().as_object_or_throw(context);
}

const std::vector<json_field>& json_field::as_object_or_throw(const std::string& context) const {
    return value().as_object_or_throw(context);
}

const std::vector<json_value>& json_field::as_array() const {
    return value().as_array();
}

std::vector<json_value>& json_field::as_array_or_throw(const std::string& context) {
    return value().as_array_or_throw(context);
}

const std::vector<json_value>& json_field::as_array_or_throw(const std::string& context) const {
    return value().as_array_or_throw(context);
}

const std::string& json_field::as_string() const {
    return value().as_string();
}

std::string& json_field::as_string_or_throw(const std::string& context) {
    return value().as_string_or_throw(context);
}

const std::string& json_field::as_string_or_throw(const std::string& context) const {
    return value().as_string_or_throw(context);
}

const std::string& json_field::as_string(const std::string& default_val) const {
    return value().as_string(default_val);
}

#ifdef STATICLIB_WITH_ICU
const icu::UnicodeString& json_field::as_ustring() const {
    return value().as_ustring();
}

const icu::UnicodeString& json_field::as_ustring_or_throw(const icu::UnicodeString& context) const {
    return value().as_ustring_or_throw(context);
}

const icu::UnicodeString& json_field::as_ustring(const icu::UnicodeString& default_val) const {
    return value().as_ustring(default_val);
}
#endif // STATICLIB_WITH_ICU

int64_t json_field::as_int64() const {
    return value().as_int64();
}

int64_t json_field::as_int64_or_throw(const std::string& context) const {
    return value().as_int64_or_throw(context);
}

int64_t json_field::as_int64(int64_t default_val) const {
    return value().as_int64(default_val);
}

int32_t json_field::as_int32() const {
    return value().as_int32();
}

int32_t json_field::as_int32_or_throw(const std::string& context) const {
    return value().as_int32_or_throw(context);
}

int32_t json_field::as_int32(int32_t default_val) const {
    return value().as_int32(default_val);
}

uint32_t json_field::as_uint32() const {
    return value().as_uint32();
}

uint32_t json_field::as_uint32_or_throw(const std::string& context) const {
    return value().as_uint32_or_throw(context);
}

uint32_t json_field::as_uint32(uint32_t default_val) const {
    return value().as_uint32(default_val);
}

int16_t json_field::as_int16() const {
    return value().as_int16();
}

int16_t json_field::as_int16_or_throw(const std::string& context) const {
    return value().as_int16_or_throw(context);
}

int16_t json_field::as_int16(int16_t default_val) const {
    return value().as_int16(default_val);
}

uint16_t json_field::as_uint16() const {
    return value().as_uint16();
}

uint16_t json_field::as_uint16_or_throw(const std::string& context) const {
    return value().as_uint16_or_throw(context);
}

uint16_t json_field::as_uint16(uint16_t default_val) const {
    return value().as_uint16(default_val);
}

double json_field::as_double() const {
    return value().as_double();
}

double json_field::as_double_or_throw(const std::string& context) const {
    return value().as_double_or_throw(context);
}

double json_field::as_double(double default_val) const {
    return value().as_double(default_val);
}

float json_field::as_float() const {
    return value().as_float();
}

float json_field::as_float_or_throw(const std::string& context) const {
    return value().as_float_or_throw(context);
}

float json_field::as_float(float default_val) const {
    return value().as_float(default_val);
}

bool json_field::as_bool() const {
    return value().as_bool();
}

bool json_field::as_bool_or_throw(const std::string& context) const {
    return value().as_bool_or_throw(context);
}

bool json_field::as_bool(bool default_val) const {
    return value().as_bool(default_val);
}

} // namespace
} 
