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

#include <vector>
#include <utility>
#include <functional>
#include <cstdint>

#ifdef STATICLIB_WITH_ICU
#include <unicode/unistr.h>
#else
#include <string>
#endif // STATICLIB_WITH_ICU

#include "staticlib/config.hpp"
#include "staticlib/ranges.hpp"

#include "staticlib/serialization/JsonType.hpp"

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
 * Access to the hold value through the wrong type (e.g. JsonValue::get_integer
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
    JsonType type;
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
#ifdef STATICLIB_WITH_ICU
        icu::UnicodeString* stringVal;
#else
        std::string* stringVal;
#endif // STATICLIB_WITH_ICU
        int64_t integerVal;
        double realVal;
        bool booleanVal;
    };
public:
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
     * Constructs `NULL_T` reflected value
     */
    JsonValue();
    
    /**
     * Constructs `NULL_T` reflected value.
     * Added for `nullptr` support in `initializer_list` literals.
     * 
     * @param nullValue nullptr
     */
    JsonValue(std::nullptr_t nullValue);

    /**
     * Constructs `OBJECT` reflected value
     * 
     * @param objectValue list of `name->value` pairs
     */
    JsonValue(std::vector<JsonField> objectValue);

    /**
     * Constructs `OBJECT` reflected value using `std::initializer_list`.
     * 
     * @param objectValue
     */
    JsonValue(const std::initializer_list<JsonField>& objectValue);
    
    /**
     * Constructs `ARRAY` reflected value
     * 
     * @param arrayValue list of reflected values
     */
    JsonValue(std::vector<JsonValue> arrayValue);

    /**
     * Constructs `ARRAY` reflected value
     * 
     * @param arrayValue range of reflected values
     */
    template <typename T>
    JsonValue(T& arrayValueRange) :
    JsonValue(emplace_values_to_vector(arrayValueRange)) { }

    /**
     * Constructs `STRING` reflected value
     * 
     * @param stringValue string value
     */
    JsonValue(const char* stringValue);
    
    /**
     * Constructs `STRING` reflected value
     * 
     * @param stringValue string value
     */
#ifdef STATICLIB_WITH_ICU
    JsonValue(icu::UnicodeString stringValue);
#else    
    JsonValue(std::string stringValue);
#endif // STATICLIB_WITH_ICU    
    
    /**
     * Constructs `INTEGER` reflected value
     * 
     * @param integerValue int value
     */
    JsonValue(int32_t integerValue);

    /**
     * Constructs `INTEGER` reflected value
     * 
     * @param integerValue int value
     */    
    JsonValue(int64_t integerValue);

    /**
     * Constructs `INTEGER` reflected value
     * 
     * @param integerValue int value
     */
    JsonValue(uint32_t integerValue);    
    
    /**
     * Constructs `REAL` reflected value
     * 
     * @param realValue double value
     */
    JsonValue(double realValue);
    
    /**
     * Constructs `BOOLEAN` reflected value
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
     * Returns type of this reflected value
     * 
     * @return type type of this reflected value
     */
    JsonType get_type() const;
    
    /**
     * Access reflected value as an `OBJECT`
     * 
     * @return list of `name->value` pairs
     */
    const std::vector<JsonField>& get_object() const;
    
    /**
     * Access reflected value as a mutable `OBJECT`
     * 
     * @return pair, first element is a pointer to the fields
     *         array or `nullptr` if this instance is not an `OBJECT`,
     *         second element is flag whether this instance is an `OBJECT`
     */
    std::pair<std::vector<JsonField>*, bool> get_object_ptr();
    
    /**
     * Returns value of the field with specified name if this
     * value is an `OBJECT` and contains specified field.
     * Otherwise returns `NULL_T` value.
     * Note: this is O(number_of_fields) operation, consider using explicit loop instead.
     * 
     * @return value of specified field
     */
#ifdef STATICLIB_WITH_ICU    
    const JsonValue& get(const icu::UnicodeString& name) const;
#else
    const JsonValue& get(const std::string& name) const;
#endif // STATICLIB_WITH_ICU
    
    /**
     * Access reflected value as an `ARRAY`
     * 
     * @return list of values
     */
    const std::vector<JsonValue>& get_array() const;

    /**
     * Access reflected value as a mutable `ARRAY`
     * 
     * @return pair, first element is a pointer to the values
     *         array or `nullptr` if this instance is not an `ARRAY`,
     *         second element is flag whether this instance is an `ARRAY`
     */
    std::pair<std::vector<JsonValue>*, bool> get_array_ptr();
    
#ifdef STATICLIB_WITH_ICU        
    /**
     * Access reflected value as an `STRING`
     * 
     * @return string value
     */
    const icu::UnicodeString& get_string() const;
    
    /**
     * Access reflected value as a `STRING`,
     * returns specified default string if this value is not a `STRING`
     * 
     * @param default_val default value
     * @return string value
     */
    const icu::UnicodeString& get_string(const icu::UnicodeString& default_val) const;
    
    /**
     * Setter for the `STRING` value
     * 
     * @param value new value
     * @return `true` if value was set, `false` if current
     *         instance is not a `STRING`
     */
    bool set_string(icu::UnicodeString value);
#else  
    /**
     * Access reflected value as an `STRING`
     * 
     * @return string value
     */
    const std::string& get_string() const;

    /**
     * Access reflected value as a `STRING`,
     * returns specified `default_val` if this value is not a `STRING`
     * 
     * @param default_val default value
     * @return string value
     */
    const std::string& get_string(const std::string& default_val) const;

    /**
     * Setter for the `STRING` value
     * 
     * @param value new value
     * @return `true` if value was set, `false` if current
     *         instance is not a `STRING`
     */
    bool set_string(std::string value);
#endif // STATICLIB_WITH_ICU    
    
    /**
     * Access reflected value as an `INTEGER`
     * 
     * @return int value
     */
    int64_t get_integer() const;

    /**
     * Access reflected value as an `INTEGER`,
     * returns specified `default_val` if this value is not an `INTEGER`
     * 
     * @param default_val default value
     * @return int value
     */
    int64_t get_integer(int64_t default_val) const;

    /**
     * Setter for the `INTEGER` value
     * 
     * @param value new value
     * @return `true` if value was set, `false` if current
     *         instance is not a `INTEGER`
     */
    bool set_integer(int64_t value);
    /**
     * Access reflected value as an `int32_t` `INTEGER`
     * 
     * @return int value
     */
    int32_t get_int32() const;

    /**
     * Access reflected value as an `int32_t` `INTEGER`,
     * returns specified `default_val` if this value is not an `INTEGER`
     * 
     * @param default_val default value
     * @return int value
     */
    int32_t get_int32(int32_t default_val) const;

    /**
     * Setter for the `INTEGER` value
     * 
     * @param value new value
     * @return `true` if value was set, `false` if current
     *         instance is not a `INTEGER`
     */
    bool set_int32(int32_t value);

    /**
     * Access reflected value as an `uint32_t` `INTEGER`
     * 
     * @return int value
     */
    uint32_t get_uint32() const;

    /**
     * Access reflected value as an `uint32_t` `INTEGER`,
     * returns specified `default_val` if this value is not an `INTEGER`
     * 
     * @param default_val default value
     * @return int value
     */
    uint32_t get_uint32(uint32_t default_val) const;

    /**
     * Setter for the `INTEGER` value
     * 
     * @param value new value
     * @return `true` if value was set, `false` if current
     *         instance is not a `INTEGER`
     */
    bool set_uint32(uint32_t value);
    
    /**
     * Access reflected value as an `int16_t` `INTEGER`
     * 
     * @return int value
     */
    int16_t get_int16() const;

    /**
     * Access reflected value as an `int16_t` `INTEGER`,
     * returns specified `default_val` if this value is not an `INTEGER`
     * 
     * @param default_val default value
     * @return int value
     */
    int16_t get_int16(int16_t default_val) const;

    /**
     * Setter for the `INTEGER` value
     * 
     * @param value new value
     * @return `true` if value was set, `false` if current
     *         instance is not a `INTEGER`
     */
    bool set_int16(int16_t value);
    
    /**
     * Access reflected value as an `uint16_t` `INTEGER`
     * 
     * @return int value
     */
    uint16_t get_uint16() const;

    /**
     * Access reflected value as an `uint16_t` `INTEGER`,
     * returns specified `default_val` if this value is not an `INTEGER`
     * 
     * @param default_val default value
     * @return int value
     */
    uint16_t get_uint16(uint16_t default_val) const;

    /**
     * Setter for the `INTEGER` value
     * 
     * @param value new value
     * @return `true` if value was set, `false` if current
     *         instance is not a `INTEGER`
     */
    bool set_uint16(uint16_t value);
    
    /**
     * Access reflected value as a `REAL`,
     * returns specified `default_val` if this value is not a `REAL`
     * 
     * @return double value
     */
    double get_real() const;

    /**
     * Access reflected value as an `REAL`,
     * 
     * @param default_val default value
     * @return double value
     */
    double get_real(double default_val) const;

    /**
     * Setter for the `REAL` value
     * 
     * @param value new value
     * @return `true` if value was set, `false` if current
     *         instance is not a `REAL`
     */
    bool set_real(double value);
    
    /**
     * Access reflected value as an `BOOLEAN`
     * 
     * @return bool value
     */
    bool get_boolean() const;

    /**
     * Access reflected value as an `BOOLEAN`,
     * returns specified `default_val` if this value is not a `BOOLEAN`
     * 
     * @param default_val default value
     * @return bool value
     */
    bool get_boolean(bool default_val) const;

    /**
     * Setter for the `BOOLEAN` value
     * 
     * @param value new value
     * @return `true` if value was set, `false` if current
     *         instance is not a `BOOLEAN`
     */
    bool set_boolean(bool value);
    
};

} // namespace
}

#endif	/* STATICLIB_SERIALIZATION_JSONVALUE_HPP */

