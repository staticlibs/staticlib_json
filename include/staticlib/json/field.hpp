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
 * File:   field.hpp
 * Author: alex
 *
 * Created on April 9, 2017, 11:58 AM
 */

#ifndef STATICLIB_JSON_FIELD_HPP
#define	STATICLIB_JSON_FIELD_HPP

#include <cstdint>
#include <string>
#include <vector>

#include "staticlib/config.hpp"

#include "staticlib/json/value.hpp"

namespace staticlib {
namespace json {

/**
 * Representation of the reflected object field. Implemented as `name, value` tuple.
 * This class mimics `value` methods like `as_array` to access field value directly 
 * from this object without intermediate `value()` call.
 */
class field {
    std::string field_name;
    value field_value;

public:
    /**
     * Deleted copy constructor
     * 
     * @param other deleted
     */
    field(const field& other) = delete;

    /**
     * Deleted copy assignment operator
     * 
     * @param other deleted
     */
    field& operator=(const field& other) = delete;

    /**
     * Move constructor
     * 
     * @param other other value
     */
    field(field&& other) STATICLIB_NOEXCEPT;

    /**
     * Move assignment operator
     * 
     * @param other other value
     */
    field& operator=(field&& other) STATICLIB_NOEXCEPT;

    /**
     * Default constructor
     */
    field();

    /**
     * Constructor
     * 
     * @param name field name
     * @param value field value
     */
    field(std::string name, value json_value);

    /**
     * Constructor
     * 
     * @param name field name
     * @param value field value
     */
    field(const char* name, value json_value);

    /**
     * Field name accessor
     * 
     * @return field name
     */
    const std::string& name() const;

    /**
     * Returns type of the value in this field
     * 
     * @return type type of the value in this field
     */
    type json_type() const;

    /**
     * Field value accessor
     * 
     * @return field value
     */
    const value& val() const;

    /**
     * Field value accessor
     * 
     * @return field value
     */
    value& val();

    /**
     * Field value mutator
     * 
     * @param value new value
     */
    void set_value(value&& value);

    /**
     * Explicit deep-copy method
     * 
     * @return deep copy of current instance
     */
    field clone() const;

    /**
     * Access value as an `OBJECT`
     * 
     * @return list of `name->value` pairs
     */
    const std::vector<field>& as_object() const;

    /**
     * Access value as a mutable `OBJECT`
     * If this value is not an `OBJECT`: "serialization_exception" will be thrown.
     * 
     * @return list of `name->value` pairs
     */
    std::vector<field>& as_object_or_throw(const std::string& context = "");

    /**
     * Access value as a mutable `OBJECT`
     * If this value is not an `OBJECT`: "serialization_exception" will be thrown.
     * 
     * @return list of `name->value` pairs
     */
    const std::vector<field>& as_object_or_throw(const std::string& context = "") const;

    /**
     * Access value as an `ARRAY`
     * 
     * @return list of values
     */
    const std::vector<value>& as_array() const;

    /**
     * Access value as a mutable `ARRAY`
     * If this value is not an `ARRAY`: "serialization_exception" will be thrown.
     * 
     * @return list of values
     */
    std::vector<value>& as_array_or_throw(const std::string& context = "");

    /**
     * Access value as a mutable `ARRAY`
     * If this value is not an `ARRAY`: "serialization_exception" will be thrown.
     * 
     * @return list of values
     */
    const std::vector<value>& as_array_or_throw(const std::string& context = "") const;

    /**
     * Access value as an `STRING`
     * 
     * @return string value
     */
    const std::string& as_string() const;

    /**
     * Access value as an `STRING`
     * If this value is not a `STRING`: "serialization_exception" will be thrown.
     * 
     * @return string value
     */
    std::string& as_string_or_throw(const std::string& context = "");

    /**
     * Access value as an `STRING`
     * If this value is not a `STRING`: "serialization_exception" will be thrown.
     * 
     * @return string value
     */
    const std::string& as_string_or_throw(const std::string& context = "") const;

    /**
     * Access value as non-empty `STRING`
     * If this value is not a non-empty `STRING`: "serialization_exception" will be thrown.
     * 
     * @return string value
     */
    std::string& as_string_nonempty_or_throw(const std::string& context = "");

    /**
     * Access value as non-empty `STRING`
     * If this value is not a non-empty `STRING`: "serialization_exception" will be thrown.
     * 
     * @return string value
     */
    const std::string& as_string_nonempty_or_throw(const std::string& context = "") const;

    /**
     * Access value as a `STRING`,
     * returns specified `default_val` if this value is not a `STRING`
     * 
     * @param default_val default value
     * @return string value
     */
    const std::string& as_string(const std::string& default_val) const;

    /**
     * Access value as an `INTEGER`
     * 
     * @return int value
     */
    int64_t as_int64() const;

    /**
     * Access value as an `INTEGER`
     * If this value is not an `INTEGER`: "serialization_exception" will be thrown.
     * 
     * @return int value
     */
    int64_t as_int64_or_throw(const std::string& context = "") const;

    /**
     * Access value as an `INTEGER`,
     * returns specified `default_val` if this value is not an `INTEGER`
     * 
     * @param default_val default value
     * @return int value
     */
    int64_t as_int64(int64_t default_val) const;

    /**
     * Access value as an `int32_t` `INTEGER`
     * 
     * @return int value
     */
    int32_t as_int32() const;

    /**
     * Access value as `int32_t` `INTEGER`
     * If this value is not an `INTEGER` or cannot be converted to `int32_t`: 
     * "serialization_exception" will be thrown.
     * 
     * @return int value
     */
    int32_t as_int32_or_throw(const std::string& context = "") const;

    /**
     * Access value as an `int32_t` `INTEGER`,
     * returns specified `default_val` if this value is not an `INTEGER`
     * 
     * @param default_val default value
     * @return int value
     */
    int32_t as_int32(int32_t default_val) const;

    /**
     * Access value as an `uint32_t` `INTEGER`
     * 
     * @return int value
     */
    uint32_t as_uint32() const;

    /**
     * Access value as `uint32_t` `INTEGER`
     * If this value is not an `INTEGER` or cannot be converted to `uint32_t`: 
     * "serialization_exception" will be thrown.
     * 
     * @return int value
     */
    uint32_t as_uint32_or_throw(const std::string& context = "") const;

    /**
     * Access value as an `uint32_t` `INTEGER`,
     * returns specified `default_val` if this value is not an `INTEGER`
     * 
     * @param default_val default value
     * @return int value
     */
    uint32_t as_uint32(uint32_t default_val) const;

    /**
     * Access value as an `int16_t` `INTEGER`
     * 
     * @return int value
     */
    int16_t as_int16() const;

    /**
     * Access value as `int16_t` `INTEGER`
     * If this value is not an `INTEGER` or cannot be converted to `int16_t`: 
     * "serialization_exception" will be thrown.
     * 
     * @return int value
     */
    int16_t as_int16_or_throw(const std::string& context = "") const;

    /**
     * Access value as an `int16_t` `INTEGER`,
     * returns specified `default_val` if this value is not an `INTEGER`
     * 
     * @param default_val default value
     * @return int value
     */
    int16_t as_int16(int16_t default_val) const;

    /**
     * Access value as an `uint16_t` `INTEGER`
     * 
     * @return int value
     */
    uint16_t as_uint16() const;

    /**
     * Access value as `uint16_t` `INTEGER`
     * If this value is not an `INTEGER` or cannot be converted to `uint16_t`: 
     * "serialization_exception" will be thrown.
     * 
     * @return int value
     */
    uint16_t as_uint16_or_throw(const std::string& context = "") const;

    /**
     * Access value as an `uint16_t` `INTEGER`,
     * returns specified `default_val` if this value is not an `INTEGER`
     * 
     * @param default_val default value
     * @return int value
     */
    uint16_t as_uint16(uint16_t default_val) const;

    /**
     * Access value as a `REAL`,
     * returns specified `default_val` if this value is not a `REAL`
     * 
     * @return double value
     */
    double as_double() const;

    /**
     * Access value as `REAL`
     * If this value is not a `REAL`: 
     * "serialization_exception" will be thrown.
     * 
     * @return int value
     */
    double as_double_or_throw(const std::string& context = "") const;

    /**
     * Access value as an `REAL`,
     * 
     * @param default_val default value
     * @return double value
     */
    double as_double(double default_val) const;

    /**
     * Access value as a `REAL`,
     * returns specified `default_val` if this value is not a `REAL`
     * 
     * @return double value
     */
    float as_float() const;

    /**
     * Access value as `float` `REAL`
     * If this value is not a `REAL` or cannot be converted to `float`: 
     * "serialization_exception" will be thrown.
     * 
     * @return int value
     */
    float as_float_or_throw(const std::string& context = "") const;

    /**
     * Access value as an `REAL`,
     * 
     * @param default_val default value
     * @return double value
     */
    float as_float(float default_val) const;

    /**
     * Access value as an `BOOLEAN`
     * 
     * @return bool value
     */
    bool as_bool() const;

    /**
     * Access value as `BOOLEAN`
     * If this value is not a `BOOLEAN`: 
     * "serialization_exception" will be thrown.
     * 
     * @return int value
     */
    bool as_bool_or_throw(const std::string& context = "") const;

    /**
     * Access value as an `BOOLEAN`,
     * returns specified `default_val` if this value is not a `BOOLEAN`
     * 
     * @param default_val default value
     * @return bool value
     */
    bool as_bool(bool default_val) const;

};

} // namespace
}

#endif	/* STATICLIB_JSON_FIELD_HPP */

