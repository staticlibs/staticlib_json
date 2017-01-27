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
 * File:   JsonValue.hpp
 * Author: alex
 *
 * Created on December 28, 2014, 8:00 PM
 */

#ifndef STATICLIB_SERIALIZATION_JSONVALUE_HPP
#define	STATICLIB_SERIALIZATION_JSONVALUE_HPP

#include <functional>
#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <cstdint>

#ifdef STATICLIB_WITH_ICU
#include <unicode/unistr.h>
#endif // STATICLIB_WITH_ICU

#include "staticlib/config.hpp"
#include "staticlib/ranges.hpp"

#include "staticlib/serialization/JsonType.hpp"
#include "staticlib/serialization/SerializationException.hpp"

namespace staticlib {
namespace serialization {

// forward declaration
class JsonField;
class JsonValue;

// utility function
namespace { //anonymous
template <typename T>
std::vector<JsonValue> emplace_values_to_vector(T& range) {
    auto vec = std::vector<JsonValue>{};
    for (auto&& el : range) {
        vec.emplace_back(std::move(el));
    }
    return vec;
}
} // namespace

/**
 * Representation of single arbitrary value for reflected classes.
 * Can represent any type of `JsonType` enum. Always hold exactly one 
 * value of the specified type (that can hold more values inside).
 * 
 * Access to the hold value through the wrong type (e.g. JsonValue::as_integer
 * on `STRING` value) is a valid operation and returns the default value of
 * the specified type. This class doesn't do automatic conversion between value types.
 * 
 * Uses pointer to vector instead of direct `std::vector` to hold recursive 
 * array and object fields of JsonValue (that is UB with `std::vector`).
 * `unique_ptr` cannot be used here instead of bare pointer because of lack of support for 
 * unrestricted unions in msvc 2013
 */
class JsonValue {
private:
    JsonType jsonType;
    // boost::variant can be used instead (and implementation will be simpler)
    // but executable will be bigger, 
    // and we do not want to leak boost::variant from this public header
    union {
        std::nullptr_t nullVal = nullptr;
        // direct std::vector is unsafe here
        // http://stackoverflow.com/q/18672135/314015
        // http://stackoverflow.com/q/8329826/314015
        std::vector<JsonField>* objectVal;
        std::vector<JsonValue>* arrayVal;
        std::string* stringVal;
        int64_t integerVal;
        double realVal;
        bool booleanVal;
    };
#ifdef STATICLIB_WITH_ICU
    mutable std::unique_ptr<icu::UnicodeString> ustringValCached;
#endif // STATICLIB_WITH_ICU
    
public:
    /**
     * Destructor
     */
    ~JsonValue() STATICLIB_NOEXCEPT;
    
    /**
     * Deleted copy constructor
     * 
     * @param other deleted
     */
    JsonValue(const JsonValue& other) = delete;

    /**
     * Deleted copy assignment operator
     * 
     * @param other deleted
     */
    JsonValue& operator=(const JsonValue& other) = delete;

    /**
     * Move constructor
     * 
     * @param other other value
     */
    JsonValue(JsonValue&& other) STATICLIB_NOEXCEPT;

    /**
     * Move assignment operator
     * 
     * @param other other value
     */
    JsonValue& operator=(JsonValue&& other) STATICLIB_NOEXCEPT;
    
    /**
     * Constructs `NULL_T` value
     */
    JsonValue();
    
    /**
     * Constructs `NULL_T` value.
     * Added for `nullptr` support in `initializer_list` literals.
     * 
     * @param nullValue nullptr
     */
    JsonValue(std::nullptr_t nullValue);

    /**
     * Constructs `OBJECT` value
     * 
     * @param objectValue list of `name->value` pairs
     */
    JsonValue(std::vector<JsonField>&& objectValue);

    /**
     * Constructs `OBJECT` value using `std::initializer_list`.
     * 
     * @param objectValue
     */
    JsonValue(const std::initializer_list<JsonField>& objectValue);
    
    /**
     * Constructs `ARRAY` value
     * 
     * @param arrayValue list of values
     */
    JsonValue(std::vector<JsonValue>&& arrayValue);

    /**
     * Constructs `ARRAY` value
     * 
     * @param arrayValue range of values
     */
    template <typename T>
    JsonValue(T& arrayValueRange) :
    JsonValue(emplace_values_to_vector(arrayValueRange)) { }
   
    /**
     * Constructs `STRING` value,
     * passed value will be copied explicitely
     * 
     * @param stringValue string value
     */
    JsonValue(const std::string& stringValue);

    /**
     * Constructs `STRING` value,
     * passed value will be copied explicitely
     * 
     * @param stringValue string value
     */
    JsonValue(std::string& stringValue);

    /**
     * Constructs `STRING` value
     * 
     * @param stringValue string value
     */
    JsonValue(std::string&& stringValue);

    /**
     * Constructs `STRING` value
     * 
     * @param stringValue string value
     */
    JsonValue(const char* stringValue);    
    
    /**
     * Constructs `STRING` value
     * 
     * @param stringValue string value
     */
#ifdef STATICLIB_WITH_ICU
    JsonValue(icu::UnicodeString ustringValue);
#endif // STATICLIB_WITH_ICU    
    
    /**
     * Constructs `INTEGER` value
     * 
     * @param integerValue int value
     */
    JsonValue(int32_t integerValue);

    /**
     * Constructs `INTEGER` value
     * 
     * @param integerValue int value
     */    
    JsonValue(int64_t integerValue);

    /**
     * Constructs `INTEGER` value
     * 
     * @param integerValue int value
     */
    JsonValue(uint32_t integerValue);

    /**
     * Constructs `INTEGER` value
     * 
     * @param integerValue int value
     */
    JsonValue(int16_t integerValue);

    /**
     * Constructs `INTEGER` value
     * 
     * @param integerValue int value
     */
    JsonValue(uint16_t integerValue);
    
    /**
     * Constructs `REAL` value
     * 
     * @param realValue double value
     */
    JsonValue(double realValue);

    /**
     * Constructs `REAL` value
     * 
     * @param realValue double value
     */
    JsonValue(float realValue);
    
    /**
     * Constructs `BOOLEAN` value
     * 
     * @param booleanValue bool value
     */
    JsonValue(bool booleanValue);

    /**
     * Explicit deep-copy method
     * 
     * @return deep copy of current instance
     */
    JsonValue clone() const;
    
    /**
     * Returns type of this value
     * 
     * @return type type of this value
     */
    JsonType type() const;
    
    /**
     * Returns value of the field with specified name if this
     * value is an `OBJECT` and contains specified field.
     * Otherwise returns `NULL_T` value.
     * Note: this is O(number_of_fields) operation, consider using explicit loop instead.
     * 
     * @return value of specified field
     */
    const JsonValue& getattr(const std::string& name) const;

    /**
     * Returns value of the field with specified name if this
     * value is an `OBJECT` and contains specified field.
     * Otherwise returns `NULL_T` value.
     * Note: this is O(number_of_fields) operation, consider using explicit loop instead.
     * 
     * @return value of specified field
     */
    const JsonValue& operator[](const std::string& name) const;
    
#ifdef STATICLIB_WITH_ICU
    /**
     * Returns value of the field with specified name if this
     * value is an `OBJECT` and contains specified field.
     * Otherwise returns `NULL_T` value.
     * Note: this is O(number_of_fields) operation, consider using explicit loop instead.
     * 
     * @return value of specified field
     */
    const JsonValue& getattru(const icu::UnicodeString& uname) const;
#endif // STATICLIB_WITH_ICU

    /**
     * Returns a mutable value of the field with specified name if this
     * value is an `OBJECT` and contains specified attribute.     
     * If this value doesn't contain specified attribute - new attribute of type `NULL_T`
     * with the specified name will be created.
     * If this value is not an `OBJECT`: "SerializationException" will be thrown.
     * Note: this is O(number_of_fields) operation, consider using explicit loop instead.
     * 
     * @return value of specified field
     */
    JsonValue& getattr_or_throw(const std::string& name, const std::string& context = "");
    
#ifdef STATICLIB_WITH_ICU
    /**
     * Returns a mutable value of the field with specified name if this
     * value is an `OBJECT` and contains specified field.
     * If this value is not an `OBJECT` it will be changed to become an empty object.
     * If this value doesn't contain specified attribute - new attribute of type `NULL_T`
     * with the specified name will be created.
     * Note: this is O(number_of_fields) operation, consider using explicit loop instead.
     * 
     * @return value of specified field
     */
    JsonValue& getattru_mutable(const icu::UnicodeString& name);
#endif // STATICLIB_WITH_ICU    
    
    /**
     * Access value as an `OBJECT`
     * 
     * @return list of `name->value` pairs
     */
    const std::vector<JsonField>& as_object() const;

    /**
     * Access value as a mutable `OBJECT`
     * If this value is not an `OBJECT`: "SerializationException" will be thrown.
     * 
     * @return list of `name->value` pairs
     */
    std::vector<JsonField>& as_object_or_throw(const std::string& context = "");

    /**
     * Access value as a mutable `OBJECT`
     * If this value is not an `OBJECT`: "SerializationException" will be thrown.
     * 
     * @return list of `name->value` pairs
     */
    const std::vector<JsonField>& as_object_or_throw(const std::string& context = "") const;

    /**
     * Setter for the `OBJECT` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `OBJECT`, `false` if current
     *         instance was changed `OBJECT`
     */
    bool set_object(std::vector<JsonField>&& value);
    
    /**
     * Access value as an `ARRAY`     
     * 
     * @return list of values
     */
    const std::vector<JsonValue>& as_array() const;

    /**
     * Access value as a mutable `ARRAY`
     * If this value is not an `ARRAY`: "SerializationException" will be thrown.
     * 
     * @return list of values
     */
    std::vector<JsonValue>& as_array_or_throw(const std::string& context = "");

    /**
     * Access value as a mutable `ARRAY`
     * If this value is not an `ARRAY`: "SerializationException" will be thrown.
     * 
     * @return list of values
     */
    const std::vector<JsonValue>& as_array_or_throw(const std::string& context = "") const;

    /**
     * Setter for the `ARRAY` value
     * 
     * @param value new value
     * @return `true` if current instance was initially a `ARRAY`, `false` if current
     *         instance was changed `ARRAY`
     */
    bool set_array(std::vector<JsonValue>&& value);    
    
    /**
     * Access value as an `STRING`
     * 
     * @return string value
     */
    const std::string& as_string() const;

    /**
     * Access value as an `STRING`
     * If this value is not a `STRING`: "SerializationException" will be thrown.
     * 
     * @return string value
     */
    std::string& as_string_or_throw(const std::string& context = "");

    /**
     * Access value as an `STRING`
     * If this value is not a `STRING`: "SerializationException" will be thrown.
     * 
     * @return string value
     */
    const std::string& as_string_or_throw(const std::string& context = "") const;

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
    bool set_string(std::string value);
    
#ifdef STATICLIB_WITH_ICU        
    /**
     * Access value as an `STRING`
     * 
     * @return string value
     */
    const icu::UnicodeString& as_ustring() const;

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
     * If this value is not an `INTEGER`: "SerializationException" will be thrown.
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
     * "SerializationException" will be thrown.
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
     * "SerializationException" will be thrown.
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
     * "SerializationException" will be thrown.
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
     * "SerializationException" will be thrown.
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
     * "SerializationException" will be thrown.
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
     * "SerializationException" will be thrown.
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
     * "SerializationException" will be thrown.
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

#endif	/* STATICLIB_SERIALIZATION_JSONVALUE_HPP */

