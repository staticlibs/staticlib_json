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

#include <vector>
#include <cstdint>

#ifdef STATICLIB_WITH_ICU
#include <unicode/unistr.h>
#else
#include <string>
#endif // STATICLIB_WITH_ICU

#include "staticlib/serialization/JsonValue.hpp"

#ifndef STATICLIB_SERIALIZATION_JSONFIELD_HPP
#define	STATICLIB_SERIALIZATION_JSONFIELD_HPP

namespace staticlib {
namespace serialization {

/**
 * Representation of the reflected object field. Implemented as `name, JsonValue` tuple.
 * This class mimics `JsonValue` methods like `get_array` to access field value directly 
 * from this object without intermediate `get_value()` call.
 */
class JsonField {
#ifdef STATICLIB_WITH_ICU
    icu::UnicodeString name;
#else    
    std::string name;
#endif // STATICLIB_WITH_ICU
    JsonValue value;
    
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
#ifdef STATICLIB_WITH_ICU
    JsonField(icu::UnicodeString name, JsonValue value);
#else
    JsonField(std::string name, JsonValue value);
#endif // STATICLIB_WITH_ICU
    
    /**
     * Field name accessor
     * 
     * @return field name
     */
#ifdef STATICLIB_WITH_ICU
    const icu::UnicodeString& get_name() const;
#else
    const std::string& get_name() const;
#endif // STATICLIB_WITH_ICU

    /**
     * Returns type of the value in this field
     * 
     * @return type type of the value in this field
     */
    JsonType get_type() const;
    
    /**
     * Field value accessor
     * 
     * @return field value
     */
    const JsonValue& get_value() const;
    
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
     * Access reflected value as an `OBJECT`
     * 
     * @return list of `name->value` pairs
     */
    const std::vector<JsonField>& get_object() const;

    /**
     * Access reflected value as an `ARRAY`
     * 
     * @return list of values
     */
    const std::vector<JsonValue>& get_array() const;

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

};

} // namespace
}

#endif	/* STATICLIB_SERIALIZATION_JSONFIELD_HPP */

