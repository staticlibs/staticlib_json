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
 * File:   JsonField.cpp
 * Author: alex
 * 
 * Created on January 23, 2015, 2:40 PM
 */

#include "staticlib/serialization/JsonField.hpp"

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

JsonField::JsonField(JsonField&& other) :
jsonName(std::move(other.jsonName)), jsonValue(std::move(other.jsonValue)) { }

JsonField& JsonField::operator=(JsonField&& other) {
    this->jsonName = std::move(other.jsonName);
    this->jsonValue = std::move(other.jsonValue);
    return *this;
}

JsonField::JsonField() { }

JsonField::JsonField(std::string name, JsonValue value) :
jsonName(std::move(name)), jsonValue(std::move(value)) { }

JsonField::JsonField(const char* name, JsonValue value) :
jsonName(name), jsonValue(std::move(value)) { }

#ifdef STATICLIB_WITH_ICU
JsonField::JsonField(icu::UnicodeString uname, JsonValue value) :
jsonName(su::to_utf8(uname)), jsonValue(std::move(value)) { }
#endif // STATICLIB_WITH_ICU

const std::string& JsonField::name() const {
    return jsonName;
}

#ifdef STATICLIB_WITH_ICU
const icu::UnicodeString& JsonField::get_uname() const {
    if (nullptr == uname.get()) {
        uname = std::unique_ptr<icu::UnicodeString>{new icu::UnicodeString{}};
        *uname = icu::UnicodeString::fromUTF8(jsonName);
    }
    return *uname;
}
#endif // STATICLIB_WITH_ICU

JsonType JsonField::type() const {
    return value().type();
}

const JsonValue& JsonField::value() const {
    return jsonValue;
}

JsonValue& JsonField::value() {
    return jsonValue;
}

void JsonField::set_value(JsonValue&& value) {
    JsonValue previous = std::move(this->jsonValue);
    this->jsonValue = std::move(value);
}

JsonField JsonField::clone() const {
    return JsonField(jsonName, jsonValue.clone());
}

const std::vector<JsonField>& JsonField::as_object() const {
    return value().as_object();
}

const std::vector<JsonValue>& JsonField::as_array() const {
    return value().as_array();
}

const std::string& JsonField::as_string() const {
    return value().as_string();
}

const std::string& JsonField::as_string(const std::string& default_val) const {
    return value().as_string(default_val);
}

#ifdef STATICLIB_WITH_ICU
const icu::UnicodeString& JsonField::get_ustring() const {
    return value().get_ustring();
}

const icu::UnicodeString& JsonField::get_ustring(const icu::UnicodeString& default_val) const {
    return value().get_ustring(default_val);
}
#endif // STATICLIB_WITH_ICU

int64_t JsonField::as_int64() const {
    return value().as_int64();
}

int64_t JsonField::as_int64(int64_t default_val) const {
    return value().as_int64(default_val);
}

int32_t JsonField::as_int32() const {
    return value().as_int32();
}

int32_t JsonField::as_int32(int32_t default_val) const {
    return value().as_int32(default_val);
}

uint32_t JsonField::as_uint32() const {
    return value().as_uint32();
}

uint32_t JsonField::as_uint32(uint32_t default_val) const {
    return value().as_uint32(default_val);
}

int16_t JsonField::as_int16() const {
    return value().as_int16();
}

int16_t JsonField::as_int16(int16_t default_val) const {
    return value().as_int16(default_val);
}

uint16_t JsonField::as_uint16() const {
    return value().as_uint16();
}

uint16_t JsonField::as_uint16(uint16_t default_val) const {
    return value().as_uint16(default_val);
}

double JsonField::as_double() const {
    return value().as_double();
}

double JsonField::as_double(double default_val) const {
    return value().as_double(default_val);
}

float JsonField::as_float() const {
    return value().as_float();
}

float JsonField::as_float(float default_val) const {
    return value().as_float(default_val);
}

bool JsonField::as_bool() const {
    return value().as_bool();
}

bool JsonField::as_bool(bool default_val) const {
    return value().as_bool(default_val);
}

} // namespace
} 
