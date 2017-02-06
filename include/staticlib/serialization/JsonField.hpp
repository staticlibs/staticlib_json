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
 * File:   JsonField.hpp
 * Author: alex
 *
 * Created on January 23, 2015, 10:33 AM
 */

#ifndef STATICLIB_SERIALIZATION_JSONFIELD_HPP
#define	STATICLIB_SERIALIZATION_JSONFIELD_HPP

#include <cstdint>
#include <string>
#include <vector>

#ifdef STATICLIB_WITH_ICU
#include <memory>
#include <unicode/unistr.h>
#endif // STATICLIB_WITH_ICU

#include "staticlib/serialization/JsonValue.hpp"

namespace staticlib {
namespace serialization {

/**
 * Representation of the reflected object field. Implemented as `name, JsonValue` tuple.
 * This class mimics `JsonValue` methods like `as_array` to access field value directly 
 * from this object without intermediate `value()` call.
 */
class JsonField {
    std::string jsonName;
#ifdef STATICLIB_WITH_ICU
    mutable std::unique_ptr<icu::UnicodeString> jsonUname;
#endif // STATICLIB_WITH_ICU
    JsonValue jsonValue;
    
public:
    /**
     * Deleted copy constructor
     * 
     * @param other deleted
     */
    JsonField(const JsonField& other) = delete;

    /**
     * Deleted copy assignment operator
     * 
     * @param other deleted
     */
    JsonField& operator=(const JsonField& other) = delete;

    /**
     * Move constructor
     * 
     * @param other other value
     */
    JsonField(JsonField&& other); /* default */

    /**
     * Move assignment operator
     * 
     * @param other other value
     */
    JsonField& operator=(JsonField&& other); /* default */
    
    /**
     * Default constructor
     */
    JsonField();
    
    /**
     * Constructor
     * 
     * @param name field name
     * @param value field value
     */
    JsonField(std::string name, JsonValue value);

    /**
     * Constructor
     * 
     * @param name field name
     * @param value field value
     */
    JsonField(const char* name, JsonValue value);

#ifdef STATICLIB_WITH_ICU
    /**
     * Constructor
     * 
     * @param name field name
     * @param value field value
     */
    JsonField(icu::UnicodeString uname, JsonValue jsonValue);
#endif // STATICLIB_WITH_ICU
    
    /**
     * Field name accessor
     * 
     * @return field name
     */
    const std::string& name() const;

#ifdef STATICLIB_WITH_ICU
    /**
     * Field name accessor
     * 
     * @return field name
     */
    const icu::UnicodeString& uname() const;
#endif // STATICLIB_WITH_ICU

    /**
     * Returns type of the value in this field
     * 
     * @return type type of the value in this field
     */
    JsonType type() const;
    
    /**
     * Field value accessor
     * 
     * @return field value
     */
    const JsonValue& value() const;

    /**
     * Field value accessor
     * 
     * @return field value
     */
    JsonValue& value();
    
    /**
     * Field value mutator
     * 
     * @param value new value
     */
    void set_value(JsonValue&& value);

    /**
     * Explicit deep-copy method
     * 
     * @return deep copy of current instance
     */
    JsonField clone() const;

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

#ifdef STATICLIB_WITH_ICU 
    /**
     * Access value as an `STRING`
     * 
     * @return string value
     */
    const icu::UnicodeString& as_ustring() const;

    /**
     * Access value as an `STRING`
     * If this value is not a `STRING`: "SerializationException" will be thrown.
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
     * Access value as a `REAL`,
     * returns specified `default_val` if this value is not a `REAL`
     * 
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
     * "SerializationException" will be thrown.
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

};

} // namespace
}

#endif	/* STATICLIB_SERIALIZATION_JSONFIELD_HPP */

