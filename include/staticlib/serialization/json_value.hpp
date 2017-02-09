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
 * File:   json_value.hpp
 * Author: alex
 *
 * Created on December 28, 2014, 8:00 PM
 */

#ifndef STATICLIB_SERIALIZATION_JSON_VALUE_HPP
#define	STATICLIB_SERIALIZATION_JSON_VALUE_HPP

#include <cstdint>
#include <string>
#include <vector>

#ifdef STATICLIB_WITH_ICU
#include <memory>
#include <unicode/unistr.h>
#endif // STATICLIB_WITH_ICU

#include "staticlib/config/noexcept.hpp"

#include "staticlib/serialization/json_type.hpp"
#include "staticlib/serialization/serialization_exception.hpp"

namespace staticlib {
namespace serialization {

// forward declaration
class json_field;
class json_value;

/**
 * Representation of single arbitrary value for reflected classes.
 * Can represent any type of `json_type` enum. Always hold exactly one 
 * value of the specified type (that can hold more values inside).
 * 
 * Access to the hold value through the wrong type (e.g. json_value::as_integer
 * on `STRING` value) is a valid operation and returns the default value of
 * the specified type. This class doesn't do automatic conversion between value types.
 * 
 * Uses pointer to vector instead of direct `std::vector` to hold recursive 
 * array and object fields of json_value (that is UB with `std::vector`).
 * `unique_ptr` cannot be used here instead of bare pointer because of lack of support for 
 * unrestricted unions in msvc 2013
 */
class json_value {
private:
    json_type value_type;
    // boost::variant can be used instead (and implementation will be simpler)
    // but executable will be bigger, 
    // and we do not want to leak boost::variant from this public header
    union {
        std::nullptr_t null_val = nullptr;
        // direct std::vector is unsafe here
        // http://stackoverflow.com/q/18672135/314015
        // http://stackoverflow.com/q/8329826/314015
        std::vector<json_field>* object_val;
        std::vector<json_value>* array_val;
        std::string* string_val;
        int64_t integer_val;
        double real_val;
        bool boolean_val;
    };
#ifdef STATICLIB_WITH_ICU
    mutable std::unique_ptr<icu::UnicodeString> ustring_val_cached;
#endif // STATICLIB_WITH_ICU
    
public:
    /**
     * Destructor
     */
    ~json_value() STATICLIB_NOEXCEPT;
    
    /**
     * Deleted copy constructor
     * 
     * @param other deleted
     */
    json_value(const json_value& other) = delete;

    /**
     * Deleted copy assignment operator
     * 
     * @param other deleted
     */
    json_value& operator=(const json_value& other) = delete;

    /**
     * Move constructor
     * 
     * @param other other value
     */
    json_value(json_value&& other) STATICLIB_NOEXCEPT;

    /**
     * Move assignment operator
     * 
     * @param other other value
     */
    json_value& operator=(json_value&& other) STATICLIB_NOEXCEPT;
    
    /**
     * Constructs `NULL_T` value
     */
    json_value();
    
    /**
     * Constructs `NULL_T` value.
     * Added for `nullptr` support in `initializer_list` literals.
     * 
     * @param null_valueue nullptr
     */
    json_value(std::nullptr_t null_valueue);

    /**
     * Constructs `OBJECT` value
     * 
     * @param object_value list of `name->value` pairs
     */
    json_value(std::vector<json_field>&& object_value);

    /**
     * Constructs `OBJECT` value using `std::initializer_list`.
     * 
     * @param object_value
     */
    json_value(const std::initializer_list<json_field>& object_value);
    
    /**
     * Constructs `ARRAY` value
     * 
     * @param array_value list of values
     */
    json_value(std::vector<json_value>&& array_value);

    /**
     * Constructs `STRING` value,
     * passed value will be copied explicitely
     * 
     * @param string_value string value
     */
    json_value(const std::string& string_value);

    /**
     * Constructs `STRING` value
     * 
     * @param string_value string value
     */
    json_value(std::string&& string_value);

    /**
     * Constructs `STRING` value
     * 
     * @param string_value string value
     */
    json_value(const char* string_value);    
    
    /**
     * Constructs `STRING` value
     * 
     * @param string_value string value
     */
#ifdef STATICLIB_WITH_ICU
    json_value(icu::UnicodeString ustring_value);
#endif // STATICLIB_WITH_ICU    
    
    /**
     * Constructs `INTEGER` value
     * 
     * @param integer_value int value
     */
    json_value(int32_t integer_value);

    /**
     * Constructs `INTEGER` value
     * 
     * @param integer_value int value
     */    
    json_value(int64_t integer_value);

    /**
     * Constructs `INTEGER` value
     * 
     * @param integer_value int value
     */
    json_value(uint32_t integer_value);

    /**
     * Constructs `INTEGER` value
     * 
     * @param integer_value int value
     */
    json_value(int16_t integer_value);

    /**
     * Constructs `INTEGER` value
     * 
     * @param integer_value int value
     */
    json_value(uint16_t integer_value);
    
    /**
     * Constructs `REAL` value
     * 
     * @param real_value double value
     */
    json_value(double real_value);

    /**
     * Constructs `REAL` value
     * 
     * @param real_value double value
     */
    json_value(float real_value);
    
    /**
     * Constructs `BOOLEAN` value
     * 
     * @param boolean_value bool value
     */
    json_value(bool boolean_value);

    /**
     * Explicit deep-copy method
     * 
     * @return deep copy of current instance
     */
    json_value clone() const;
    
    /**
     * Returns type of this value
     * 
     * @return type type of this value
     */
    json_type type() const;
    
    /**
     * Returns value of the field with specified name if this
     * value is an `OBJECT` and contains specified field.
     * Otherwise returns `NULL_T` value.
     * Note: this is O(number_of_fields) operation, consider using explicit loop instead.
     * 
     * @return value of specified field
     */
    const json_value& getattr(const std::string& name) const;

    /**
     * Returns value of the field with specified name if this
     * value is an `OBJECT` and contains specified field.
     * Otherwise returns `NULL_T` value.
     * Note: this is O(number_of_fields) operation, consider using explicit loop instead.
     * 
     * @return value of specified field
     */
    const json_value& operator[](const std::string& name) const;
    
#ifdef STATICLIB_WITH_ICU
    /**
     * Returns value of the field with specified name if this
     * value is an `OBJECT` and contains specified field.
     * Otherwise returns `NULL_T` value.
     * Note: this is O(number_of_fields) operation, consider using explicit loop instead.
     * 
     * @return value of specified field
     */
    const json_value& getattru(const icu::UnicodeString& uname) const;
#endif // STATICLIB_WITH_ICU

    /**
     * Returns a mutable value of the field with specified name if this
     * value is an `OBJECT` and contains specified attribute.     
     * If this value doesn't contain specified attribute - new attribute of type `NULL_T`
     * with the specified name will be created.
     * If this value is not an `OBJECT`: "serialization_exception" will be thrown.
     * Note: this is O(number_of_fields) operation, consider using explicit loop instead.
     * 
     * @return value of specified field
     */
    json_value& getattr_or_throw(const std::string& name, const std::string& context = "");
    
#ifdef STATICLIB_WITH_ICU
    /**
     * Returns a mutable value of the field with specified name if this
     * value is an `OBJECT` and contains specified attribute.     
     * If this value doesn't contain specified attribute - new attribute of type `NULL_T`
     * with the specified name will be created.
     * If this value is not an `OBJECT`: "serialization_exception" will be thrown.
     * Note: this is O(number_of_fields) operation, consider using explicit loop instead.
     * 
     * @return value of specified field
     */
    json_value& getattru_or_throw(const icu::UnicodeString& name, const icu::UnicodeString& context = "");
#endif // STATICLIB_WITH_ICU    
    
    /**
     * Access value as an `OBJECT`
     * 
     * @return list of `name->value` pairs
     */
    const std::vector<json_field>& as_object() const;

    /**
     * Access value as a mutable `OBJECT`
     * If this value is not an `OBJECT`: "serialization_exception" will be thrown.
     * 
     * @return list of `name->value` pairs
     */
    std::vector<json_field>& as_object_or_throw(const std::string& context = "");

    /**
     * Access value as a mutable `OBJECT`
     * If this value is not an `OBJECT`: "serialization_exception" will be thrown.
     * 
     * @return list of `name->value` pairs
     */
    const std::vector<json_field>& as_object_or_throw(const std::string& context = "") const;

    /**
     * Setter for the `OBJECT` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `OBJECT`, `false` if current
     *         instance was changed `OBJECT`
     */
    bool set_object(std::vector<json_field>&& value);
    
    /**
     * Access value as an `ARRAY`     
     * 
     * @return list of values
     */
    const std::vector<json_value>& as_array() const;

    /**
     * Access value as a mutable `ARRAY`
     * If this value is not an `ARRAY`: "serialization_exception" will be thrown.
     * 
     * @return list of values
     */
    std::vector<json_value>& as_array_or_throw(const std::string& context = "");

    /**
     * Access value as a mutable `ARRAY`
     * If this value is not an `ARRAY`: "serialization_exception" will be thrown.
     * 
     * @return list of values
     */
    const std::vector<json_value>& as_array_or_throw(const std::string& context = "") const;

    /**
     * Setter for the `ARRAY` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `ARRAY`, `false` if current
     *         instance was changed `ARRAY`
     */
    bool set_array(std::vector<json_value>&& value);    
    
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
     * Access value as an `STRING`
     * If this value is not a `STRING`: "serialization_exception" will be thrown.
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
    bool set_string(const std::string& value);
    
    /**
     * Setter for the `STRING` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `STRING`, `false` if current
     *         instance was changed `STRING`
     */
    bool set_string(std::string&& value);
    
#ifdef STATICLIB_WITH_ICU        
    /**
     * Access value as an `STRING`
     * 
     * @return string value
     */
    const icu::UnicodeString& as_ustring() const;

    /**
     * Access value as an `STRING`
     * If this value is not a `STRING`: "serialization_exception" will be thrown.
     * 
     * @return string value
     */
    const icu::UnicodeString& as_ustring_or_throw(const icu::UnicodeString& context = "") const;
    
    /**
     * Access value as a `STRING`,
     * returns specified default string if this value is not a `STRING`
     * 
     * @param default_val default value
     * @return string value
     */
    const icu::UnicodeString& as_ustring(const icu::UnicodeString& default_val) const;

    /**
     * Setter for the `STRING` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `STRING`, `false` if current
     *         instance was changed `STRING`
     */
    bool set_ustring(icu::UnicodeString value);
#endif // STATICLIB_WITH_ICU
    
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
     * Setter for the `INTEGER` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `INTEGER`, `false` if current
     *         instance was changed `INTEGER`
     */
    bool set_int64(int64_t value);
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
     * Setter for the `INTEGER` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `INTEGER`, `false` if current
     *         instance was changed `INTEGER`
     */
    bool set_int32(int32_t value);

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
     * Access value as positive `uint32_t` `INTEGER`
     * If this value is not a positive `INTEGER` or cannot be converted to `uint32_t`: 
     * "serialization_exception" will be thrown.
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
    bool set_uint32(uint32_t value);
    
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
     * Setter for the `INTEGER` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `INTEGER`, `false` if current
     *         instance was changed `INTEGER`
     */
    bool set_int16(int16_t value);
    
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
     * Access value as positive `uint16_t` `INTEGER`
     * If this value is not a positive `INTEGER` or cannot be converted to `uint16_t`: 
     * "serialization_exception" will be thrown.
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
    bool set_uint16(uint16_t value);

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
     * "serialization_exception" will be thrown.
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
    bool set_double(double value);

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
     * "serialization_exception" will be thrown.
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
    bool set_float(float value);    
    
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

    /**
     * Setter for the `BOOLEAN` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `BOOLEAN`, `false` if current
     *         instance was changed `BOOLEAN`
     */
    bool set_bool(bool value);
    
};

} // namespace
}

#endif	/* STATICLIB_SERIALIZATION_JSON_VALUE_HPP */

