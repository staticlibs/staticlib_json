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
 * File:   value.hpp
 * Author: alex
 *
 * Created on April 9, 2017, 11:58 AM
 */

#ifndef STATICLIB_JSON_VALUE_HPP
#define	STATICLIB_JSON_VALUE_HPP

#include <cstdint>
#include <streambuf>
#include <string>
#include <vector>

#include "staticlib/config.hpp"
#include "staticlib/io.hpp"

#include "staticlib/json/type.hpp"
#include "staticlib/json/json_exception.hpp"

namespace staticlib {
namespace json {

// forward declaration
class field;
class value;

/**
 * Representation of single arbitrary value for reflected classes.
 * Can represent any type of `type` enum. Always hold exactly one 
 * value of the specified type (that can hold more values inside).
 * 
 * Access to the hold value through the wrong type (e.g. value::as_integer
 * on `STRING` value) is a valid operation and returns the default value of
 * the specified type. This class doesn't do automatic conversion between value types.
 * 
 * Uses pointer to vector instead of direct `std::vector` to hold recursive 
 * array and object fields of value (that is UB with `std::vector`).
 * `unique_ptr` cannot be used here instead of bare pointer because of lack of support for 
 * unrestricted unions in msvc 2013
 */
class value {
private:
    type value_type;
    // boost::variant can be used instead (and implementation will be simpler)
    // but executable will be bigger, 
    // and we do not want to leak boost::variant from this public header

    union {
        std::nullptr_t null_val = nullptr;
        // direct std::vector is unsafe here
        // http://stackoverflow.com/q/18672135/314015
        // http://stackoverflow.com/q/8329826/314015
        std::vector<field>* object_val;
        std::vector<value>* array_val;
        std::string* string_val;
        int64_t integer_val;
        double real_val;
        bool boolean_val;
    };

public:
    /**
     * Destructor
     */
    ~value() STATICLIB_NOEXCEPT;

    /**
     * Deleted copy constructor
     * 
     * @param other deleted
     */
    value(const value& other) = delete;

    /**
     * Deleted copy assignment operator
     * 
     * @param other deleted
     */
    value& operator=(const value& other) = delete;

    /**
     * Move constructor
     * 
     * @param other other value
     */
    value(value&& other) STATICLIB_NOEXCEPT;

    /**
     * Move assignment operator
     * 
     * @param other other value
     */
    value& operator=(value&& other) STATICLIB_NOEXCEPT;

    /**
     * Constructs `NULL_T` value
     */
    value();

    /**
     * Constructs `NULL_T` value.
     * Added for `nullptr` support in `initializer_list` literals.
     * 
     * @param null_valueue nullptr
     */
    value(std::nullptr_t null_valueue);

    /**
     * Constructs `OBJECT` value
     * 
     * @param object_value list of `name->value` pairs
     */
    value(std::vector<field>&& object_value);

    /**
     * Constructs `OBJECT` value using `std::initializer_list`.
     * 
     * @param object_value
     */
    value(const std::initializer_list<field>& object_value);

    /**
     * Constructs `ARRAY` value
     * 
     * @param array_value list of values
     */
    value(std::vector<value>&& array_value);

    /**
     * Constructs `STRING` value,
     * passed value will be copied explicitely
     * 
     * @param string_value string value
     */
    value(const std::string& string_value);

    /**
     * Constructs `STRING` value
     * 
     * @param string_value string value
     */
    value(std::string&& string_value);

    /**
     * Constructs `STRING` value
     * 
     * @param string_value string value
     */
    value(const char* string_value);

    /**
     * Constructs `INTEGER` value
     * 
     * @param integer_value int value
     */
    value(int32_t integer_value);

    /**
     * Constructs `INTEGER` value
     * 
     * @param integer_value int value
     */
    value(int64_t integer_value);

    /**
     * Constructs `INTEGER` value
     * 
     * @param integer_value int value
     */
    value(uint32_t integer_value);

    /**
     * Constructs `INTEGER` value
     * 
     * @param integer_value int value
     */
    value(int16_t integer_value);

    /**
     * Constructs `INTEGER` value
     * 
     * @param integer_value int value
     */
    value(uint16_t integer_value);

    /**
     * Constructs `REAL` value
     * 
     * @param real_value double value
     */
    value(double real_value);

    /**
     * Constructs `REAL` value
     * 
     * @param real_value double value
     */
    value(float real_value);

    /**
     * Constructs `BOOLEAN` value
     * 
     * @param boolean_value bool value
     */
    value(bool boolean_value);

    
    /**
     * Serializes this instance
     * to JSON. JSON is written to the specified streambuf. Preserves order of object fields.
     * 
     * @param dest streambuf to write JSON into
     * @return JSON string
     * @throws json_exception
     */
    void dump(std::streambuf* dest) const;

    /**
     * Serializes this instance to JSON. 
     * JSON is written to the specified sink. Preserves order of object fields.
     * 
     * @param dest sink to write JSON into
     * @return JSON string
     * @throws json_exception
     */
    template <typename Sink>
    void dump(Sink& dest) const {
        auto sbuf = sl::io::make_unbuffered_ostreambuf(dest);
        dump(std::addressof(sbuf));
    }

    /**
     * Serializes this instance
     * to JSON string. Preserves order of object fields.
     * 
     * @return JSON string
     * @throws json_exception
     */
    std::string dumps() const;   
    
    /**
     * Explicit deep-copy method
     * 
     * @return deep copy of current instance
     */
    value clone() const;

    /**
     * Returns type of this value
     * 
     * @return type type of this value
     */
    type json_type() const;

    /**
     * Returns value of the field with specified name if this
     * value is an `OBJECT` and contains specified field.
     * Otherwise returns `NULL_T` value.
     * Note: this is O(number_of_fields) operation, consider using explicit loop instead.
     * 
     * @return value of specified field
     */
    const value& getattr(const std::string& name) const;

    /**
     * Returns value of the field with specified name if this
     * value is an `OBJECT` and contains specified field.
     * Otherwise returns `NULL_T` value.
     * Note: this is O(number_of_fields) operation, consider using explicit loop instead.
     * 
     * @return value of specified field
     */
    const value& operator[](const std::string& name) const;

    /**
     * Returns a mutable value of the field with specified name if this
     * value is an `OBJECT` and contains specified attribute.     
     * If this value doesn't contain specified attribute - new attribute of type `NULL_T`
     * with the specified name will be created.
     * If this value is not an `OBJECT`: "json_exception" will be thrown.
     * Note: this is O(number_of_fields) operation, consider using explicit loop instead.
     * 
     * @return value of specified field
     */
    value& getattr_or_throw(const std::string& name, const std::string& context = "");

    /**
     * Access value as an `OBJECT`
     * 
     * @return list of `name->value` pairs
     */
    const std::vector<field>& as_object() const;

    /**
     * Access value as a mutable `OBJECT`
     * If this value is not an `OBJECT`: "json_exception" will be thrown.
     * 
     * @return list of `name->value` pairs
     */
    std::vector<field>& as_object_or_throw(const std::string& context = "");

    /**
     * Access value as a mutable `OBJECT`
     * If this value is not an `OBJECT`: "json_exception" will be thrown.
     * 
     * @return list of `name->value` pairs
     */
    const std::vector<field>& as_object_or_throw(const std::string& context = "") const;

    /**
     * Setter for the `OBJECT` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `OBJECT`, `false` if current
     *         instance was changed `OBJECT`
     */
    bool set_object(std::vector<field>&& object_value);

    /**
     * Access value as an `ARRAY`     
     * 
     * @return list of values
     */
    const std::vector<value>& as_array() const;

    /**
     * Access value as a mutable `ARRAY`
     * If this value is not an `ARRAY`: "json_exception" will be thrown.
     * 
     * @return list of values
     */
    std::vector<value>& as_array_or_throw(const std::string& context = "");

    /**
     * Access value as a mutable `ARRAY`
     * If this value is not an `ARRAY`: "json_exception" will be thrown.
     * 
     * @return list of values
     */
    const std::vector<value>& as_array_or_throw(const std::string& context = "") const;

    /**
     * Setter for the `ARRAY` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `ARRAY`, `false` if current
     *         instance was changed `ARRAY`
     */
    bool set_array(std::vector<value>&& array_value);

    /**
     * Access value as an `STRING`
     * 
     * @return string value
     */
    const std::string& as_string() const;

    /**
     * Access value as an `STRING`
     * If this value is not a `STRING`: "json_exception" will be thrown.
     * 
     * @return string value
     */
    std::string& as_string_or_throw(const std::string& context = "");

    /**
     * Access value as an `STRING`
     * If this value is not a `STRING`: "json_exception" will be thrown.
     * 
     * @return string value
     */
    const std::string& as_string_or_throw(const std::string& context = "") const;

    /**
     * Access value as non-empty `STRING`
     * If this value is not a non-empty `STRING`: "json_exception" will be thrown.
     * 
     * @return string value
     */
    std::string& as_string_nonempty_or_throw(const std::string& context = "");

    /**
     * Access value as an `STRING`
     * If this value is not a `STRING`: "json_exception" will be thrown.
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
     * Setter for the `STRING` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `STRING`, `false` if current
     *         instance was changed `STRING`
     */
    bool set_string(const std::string& string_value);

    /**
     * Setter for the `STRING` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `STRING`, `false` if current
     *         instance was changed `STRING`
     */
    bool set_string(std::string&& string_value);

    /**
     * Access value as an `INTEGER`
     * 
     * @return int value
     */
    int64_t as_int64() const;

    /**
     * Access value as an `INTEGER`
     * If this value is not an `INTEGER`: "json_exception" will be thrown.
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
     * Setter for the `INTEGER` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `INTEGER`, `false` if current
     *         instance was changed `INTEGER`
     */
    bool set_int64(int64_t int_value);
    /**
     * Access value as an `int32_t` `INTEGER`
     * 
     * @return int value
     */
    int32_t as_int32() const;

    /**
     * Access value as `int32_t` `INTEGER`
     * If this value is not an `INTEGER` or cannot be converted to `int32_t`: 
     * "json_exception" will be thrown.
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
     * Setter for the `INTEGER` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `INTEGER`, `false` if current
     *         instance was changed `INTEGER`
     */
    bool set_int32(int32_t int_value);

    /**
     * Access value as an `uint32_t` `INTEGER`
     * 
     * @return int value
     */
    uint32_t as_uint32() const;

    /**
     * Access value as `uint32_t` `INTEGER`
     * If this value is not an `INTEGER` or cannot be converted to `uint32_t`: 
     * "json_exception" will be thrown.
     * 
     * @return int value
     */
    uint32_t as_uint32_or_throw(const std::string& context = "") const;

    /**
     * Access value as positive `uint32_t` `INTEGER`
     * If this value is not a positive `INTEGER` or cannot be converted to `uint32_t`: 
     * "json_exception" will be thrown.
     * 
     * @return int value
     */
    uint32_t as_uint32_positive_or_throw(const std::string& context = "") const;

    /**
     * Access value as an `uint32_t` `INTEGER`,
     * returns specified `default_val` if this value is not an `INTEGER`
     * 
     * @param default_val default value
     * @return int value
     */
    uint32_t as_uint32(uint32_t default_val) const;

    /**
     * Setter for the `INTEGER` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `INTEGER`, `false` if current
     *         instance was changed `INTEGER`
     */
    bool set_uint32(uint32_t uint_value);

    /**
     * Access value as an `int16_t` `INTEGER`
     * 
     * @return int value
     */
    int16_t as_int16() const;

    /**
     * Access value as `int16_t` `INTEGER`
     * If this value is not an `INTEGER` or cannot be converted to `int16_t`: 
     * "json_exception" will be thrown.
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
     * Setter for the `INTEGER` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `INTEGER`, `false` if current
     *         instance was changed `INTEGER`
     */
    bool set_int16(int16_t int_value);

    /**
     * Access value as an `uint16_t` `INTEGER`
     * 
     * @return int value
     */
    uint16_t as_uint16() const;

    /**
     * Access value as `uint16_t` `INTEGER`
     * If this value is not an `INTEGER` or cannot be converted to `uint16_t`: 
     * "json_exception" will be thrown.
     * 
     * @return int value
     */
    uint16_t as_uint16_or_throw(const std::string& context = "") const;

    /**
     * Access value as positive `uint16_t` `INTEGER`
     * If this value is not a positive `INTEGER` or cannot be converted to `uint16_t`: 
     * "json_exception" will be thrown.
     * 
     * @return int value
     */
    uint16_t as_uint16_positive_or_throw(const std::string& context = "") const;

    /**
     * Access value as an `uint16_t` `INTEGER`,
     * returns specified `default_val` if this value is not an `INTEGER`
     * 
     * @param default_val default value
     * @return int value
     */
    uint16_t as_uint16(uint16_t default_val) const;

    /**
     * Setter for the `INTEGER` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `INTEGER`, `false` if current
     *         instance was changed `INTEGER`
     */
    bool set_uint16(uint16_t uint_value);

    /**
     * Access value as an `REAL`,
     * 
     * @param default_val default value
     * @return double value
     */
    double as_double() const;

    /**
     * Access value as `REAL`
     * If this value is not a `REAL`: 
     * "json_exception" will be thrown.
     * 
     * @return int value
     */
    double as_double_or_throw(const std::string& context = "") const;

    /**
     * Access value as a `REAL`,
     * returns specified `default_val` if this value is not a `REAL`
     * 
     * @return double value
     */
    double as_double(double default_val) const;

    /**
     * Setter for the `REAL` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `REAL`, `false` if current
     *         instance was changed `REAL`
     */
    bool set_double(double double_value);

    /**
     * Access value as a `float` `REAL`,
     * 
     * @param default_val default value
     * @return double value
     */
    float as_float() const;

    /**
     * Access value as `float` `REAL`
     * If this value is not a `REAL` or cannot be converted to `float`: 
     * "json_exception" will be thrown.
     * 
     * @return int value
     */
    float as_float_or_throw(const std::string& context = "") const;

    /**
     * Access value as a `float` `REAL`,
     * 
     * @return double value
     */
    float as_float(float default_val) const;

    /**
     * Setter for the `REAL` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `REAL`, `false` if current
     *         instance was changed `REAL`
     */
    bool set_float(float float_value);

    /**
     * Access value as an `BOOLEAN`
     * 
     * @return bool value
     */
    bool as_bool() const;

    /**
     * Access value as `BOOLEAN`
     * If this value is not a `BOOLEAN`: 
     * "json_exception" will be thrown.
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

    /**
     * Setter for the `BOOLEAN` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `BOOLEAN`, `false` if current
     *         instance was changed `BOOLEAN`
     */
    bool set_bool(bool bool_value);

};

} // namespace
}

#endif	/* STATICLIB_JSON_VALUE_HPP */

