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
 * File:   field.cpp
 * Author: alex
 *
 * Created on April 9, 2017, 5:51 PM
 */

#include "staticlib/json/field.hpp"

namespace staticlib {
namespace json {

field::field(field&& other) STATICLIB_NOEXCEPT :
field_name(std::move(other.field_name)),
field_value(std::move(other.field_value)) { }

field& field::operator=(field&& other) STATICLIB_NOEXCEPT {
    field_name = std::move(other.field_name);
    field_value = std::move(other.field_value);
    return *this;
}

field::field() { }

field::field(std::string name, value json_value) :
field_name(std::move(name)),
field_value(std::move(json_value)) { }

field::field(const char* name, value json_value) :
field_name(name),
field_value(std::move(json_value)) { }

const std::string& field::name() const {
    return field_name;
}

type field::json_type() const {
    return val().json_type();
}

const value& field::val() const {
    return field_value;
}

value& field::val() {
    return field_value;
}

void field::set_value(value&& json_value) {
    value previous = std::move(this->field_value);
    this->field_value = std::move(json_value);
}

field field::clone() const {
    return field(field_name, field_value.clone());
}

const std::vector<field>& field::as_object() const {
    return val().as_object();
}

std::vector<field>& field::as_object_or_throw(const std::string& context) {
    return val().as_object_or_throw(context);
}

const std::vector<field>& field::as_object_or_throw(const std::string& context) const {
    return val().as_object_or_throw(context);
}

const std::vector<value>& field::as_array() const {
    return val().as_array();
}

std::vector<value>& field::as_array_or_throw(const std::string& context) {
    return val().as_array_or_throw(context);
}

const std::vector<value>& field::as_array_or_throw(const std::string& context) const {
    return val().as_array_or_throw(context);
}

const std::string& field::as_string() const {
    return val().as_string();
}

std::string& field::as_string_or_throw(const std::string& context) {
    return val().as_string_or_throw(context);
}

const std::string& field::as_string_or_throw(const std::string& context) const {
    return val().as_string_or_throw(context);
}

std::string& field::as_string_nonempty_or_throw(const std::string& context) {
    return val().as_string_nonempty_or_throw(context);
}

const std::string& field::as_string_nonempty_or_throw(const std::string& context) const {
    return val().as_string_nonempty_or_throw(context);
}

const std::string& field::as_string(const std::string& default_val) const {
    return val().as_string(default_val);
}

int64_t field::as_int64() const {
    return val().as_int64();
}

int64_t field::as_int64_or_throw(const std::string& context) const {
    return val().as_int64_or_throw(context);
}

int64_t field::as_int64(int64_t default_val) const {
    return val().as_int64(default_val);
}

int32_t field::as_int32() const {
    return val().as_int32();
}

int32_t field::as_int32_or_throw(const std::string& context) const {
    return val().as_int32_or_throw(context);
}

int32_t field::as_int32(int32_t default_val) const {
    return val().as_int32(default_val);
}

uint32_t field::as_uint32() const {
    return val().as_uint32();
}

uint32_t field::as_uint32_or_throw(const std::string& context) const {
    return val().as_uint32_or_throw(context);
}

uint32_t field::as_uint32_positive_or_throw(const std::string& context) const {
    return val().as_uint32_positive_or_throw(context);
}

uint32_t field::as_uint32(uint32_t default_val) const {
    return val().as_uint32(default_val);
}

int16_t field::as_int16() const {
    return val().as_int16();
}

int16_t field::as_int16_or_throw(const std::string& context) const {
    return val().as_int16_or_throw(context);
}

int16_t field::as_int16(int16_t default_val) const {
    return val().as_int16(default_val);
}

uint16_t field::as_uint16() const {
    return val().as_uint16();
}

uint16_t field::as_uint16_or_throw(const std::string& context) const {
    return val().as_uint16_or_throw(context);
}

uint16_t field::as_uint16_positive_or_throw(const std::string& context) const {
    return val().as_uint16_positive_or_throw(context);
}

uint16_t field::as_uint16(uint16_t default_val) const {
    return val().as_uint16(default_val);
}

double field::as_double() const {
    return val().as_double();
}

double field::as_double_or_throw(const std::string& context) const {
    return val().as_double_or_throw(context);
}

double field::as_double(double default_val) const {
    return val().as_double(default_val);
}

float field::as_float() const {
    return val().as_float();
}

float field::as_float_or_throw(const std::string& context) const {
    return val().as_float_or_throw(context);
}

float field::as_float(float default_val) const {
    return val().as_float(default_val);
}

bool field::as_bool() const {
    return val().as_bool();
}

bool field::as_bool_or_throw(const std::string& context) const {
    return val().as_bool_or_throw(context);
}

bool field::as_bool(bool default_val) const {
    return val().as_bool(default_val);
}

} // namespace
} 


