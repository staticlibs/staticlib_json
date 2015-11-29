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

namespace staticlib {
namespace serialization {

JsonField::JsonField(JsonField&& other) :
name(std::move(other.name)), value(std::move(other.value)) { }

JsonField& JsonField::operator=(JsonField&& other) {
    this->name = std::move(other.name);
    this->value = std::move(other.value);
    return *this;
}

JsonField::JsonField() { }

#ifdef STATICLIB_WITH_ICU
JsonField::JsonField(icu::UnicodeString name, JsonValue value) :
name(std::move(name)), value(std::move(value)) { }
#else
JsonField::JsonField(std::string name, JsonValue value) :
name(std::move(name)), value(std::move(value)) { }
#endif // STATICLIB_WITH_ICU

#ifdef STATICLIB_WITH_ICU
const icu::UnicodeString& JsonField::get_name() const {
    return name;
}
#else
const std::string& JsonField::get_name() const {
    return name;
}
#endif // STATICLIB_WITH_ICU

JsonType JsonField::get_type() const {
    return get_value().get_type();
}

const JsonValue& JsonField::get_value() const {
    return value;
}

void JsonField::set_value(JsonValue&& value) {
    JsonValue previous = std::move(this->value);
    this->value = std::move(value);
}

JsonField JsonField::clone() const {
    return JsonField(name, value.clone());
}

const std::vector<JsonField>& JsonField::get_object() const {
    return get_value().get_object();
}

const std::vector<JsonValue>& JsonField::get_array() const {
    return get_value().get_array();
}

#ifdef STATICLIB_WITH_ICU
const icu::UnicodeString& JsonField::get_string() const {
    return get_value().get_string();
}

const icu::UnicodeString& JsonField::get_string(const icu::UnicodeString& default_val) const {
    return get_value().get_string(default_val);
}
#else
const std::string& JsonField::get_string() const {
    return get_value().get_string();
}

const std::string& JsonField::get_string(const std::string& default_val) const {
    return get_value().get_string(default_val);
}
#endif // STATICLIB_WITH_ICU

int64_t JsonField::get_integer() const {
    return get_value().get_integer();
}

int64_t JsonField::get_integer(int64_t default_val) const {
    return get_value().get_integer(default_val);
}

int32_t JsonField::get_int32() const {
    return get_value().get_int32();
}

int32_t JsonField::get_int32(int32_t default_val) const {
    return get_value().get_int32(default_val);
}

uint32_t JsonField::get_uint32() const {
    return get_value().get_uint32();
}

uint32_t JsonField::get_uint32(uint32_t default_val) const {
    return get_value().get_uint32(default_val);
}

int16_t JsonField::get_int16() const {
    return get_value().get_int16();
}

int16_t JsonField::get_int16(int16_t default_val) const {
    return get_value().get_int16(default_val);
}

uint16_t JsonField::get_uint16() const {
    return get_value().get_uint16();
}

uint16_t JsonField::get_uint16(uint16_t default_val) const {
    return get_value().get_uint16(default_val);
}

double JsonField::get_real() const {
    return get_value().get_real();
}

double JsonField::get_real(double default_val) const {
    return get_value().get_real(default_val);
}

bool JsonField::get_boolean() const {
    return get_value().get_boolean();
}

bool JsonField::get_boolean(bool default_val) const {
    return get_value().get_boolean(default_val);
}

} // namespace
} 
