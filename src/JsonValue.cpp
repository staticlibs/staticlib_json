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
 * File:   JsonValue.cpp
 * Author: alex
 * 
 * Created on December 28, 2014, 8:38 PM
 */

#include "staticlib/serialization/JsonValue.hpp"

#include "staticlib/serialization/JsonField.hpp"

namespace staticlib {
namespace serialization {

namespace { // anonymous

const std::vector<JsonField> EMPTY_OBJECT{};
const std::vector<JsonValue> EMPTY_ARRAY{};
#ifdef STATICLIB_WITH_ICU
const icu::UnicodeString EMPTY_STRING{};
#else
const std::string EMPTY_STRING{};
#endif // STATICLIB_WITH_ICU
const JsonValue NULL_VALUE{};

} // namespace

JsonValue::~JsonValue() STATICLIB_NOEXCEPT {
    switch (this->type) {
    case JsonType::NULL_T: break;
    case JsonType::OBJECT: delete this->objectVal; break;
    case JsonType::ARRAY: delete this->arrayVal; break;
    case JsonType::STRING: delete this->stringVal; break;
    case JsonType::INTEGER: break;
    case JsonType::REAL: break;
    case JsonType::BOOLEAN: break;
    }
}

JsonValue::JsonValue(JsonValue&& other) STATICLIB_NOEXCEPT {
    switch (other.type) {
    case JsonType::NULL_T: break;
    case JsonType::OBJECT: this->objectVal = other.objectVal; break;
    case JsonType::ARRAY: this->arrayVal = other.arrayVal; break;
    case JsonType::STRING: this->stringVal = other.stringVal; break;
    case JsonType::INTEGER: this->integerVal = other.integerVal; break;
    case JsonType::REAL: this->realVal = other.realVal; break;
    case JsonType::BOOLEAN: this->booleanVal = other.booleanVal; break;
    }
    this->type = other.type;
    // moved from object is empty now
    other.type = JsonType::NULL_T;
}

JsonValue& JsonValue::operator=(JsonValue&& other) STATICLIB_NOEXCEPT {
    // destroy existing value
    switch (this->type) {
    case JsonType::NULL_T: break;
    case JsonType::OBJECT: delete this->objectVal; break;
    case JsonType::ARRAY: delete this->arrayVal; break;
    case JsonType::STRING: delete this->stringVal; break;
    case JsonType::INTEGER: break;
    case JsonType::REAL: break;
    case JsonType::BOOLEAN: break;
    }
    // assign new value
    switch (other.type) {
    case JsonType::NULL_T: this->nullVal = other.nullVal; break;
    case JsonType::OBJECT: this->objectVal = other.objectVal; break;
    case JsonType::ARRAY: this->arrayVal = other.arrayVal; break;
    case JsonType::STRING: this->stringVal = other.stringVal; break;
    case JsonType::INTEGER: this->integerVal = other.integerVal; break;
    case JsonType::REAL: this->realVal = other.realVal; break;
    case JsonType::BOOLEAN: this->booleanVal = other.booleanVal; break;
    }
    this->type = other.type;
    // moved from object is empty now
    other.type = JsonType::NULL_T;
    return *this;
}

JsonValue::JsonValue() :
type(JsonType::NULL_T) { }

JsonValue::JsonValue(std::nullptr_t nullValue) :
type(JsonType::NULL_T) { (void) nullValue; }

JsonValue::JsonValue(std::vector<JsonField> objectValue) :
type(JsonType::OBJECT) { 
    // allocate empty vector and move data into it
    this->objectVal = new std::vector<JsonField>{};
    *(this->objectVal) = std::move(objectValue);
}

JsonValue::JsonValue(const std::initializer_list<JsonField>& objectValue) :
type(JsonType::OBJECT) {
    this->objectVal = new std::vector<JsonField>{};
    for (auto& a : objectValue) {
        this->objectVal->emplace_back(a.get_name(), a.get_value().clone());
    }
}

JsonValue::JsonValue(std::vector<JsonValue> arrayValue) :
type(JsonType::ARRAY) { 
    // allocate empty vector and move data into it
    this->arrayVal = new std::vector<JsonValue>{};
    *(this->arrayVal) = std::move(arrayValue);
}

#ifdef STATICLIB_WITH_ICU
JsonValue::JsonValue(icu::UnicodeString stringValue) :
type(JsonType::STRING) {
    this->stringVal = new icu::UnicodeString();
    *(this->stringVal) = std::move(stringValue);
}
#else
JsonValue::JsonValue(std::string stringValue) :
type(JsonType::STRING) { 
    this->stringVal = new std::string();
    *(this->stringVal) = std::move(stringValue);
}
#endif // STATICLIB_WITH_ICU


JsonValue::JsonValue(const char* stringValue) :
type(JsonType::STRING) {
#ifdef STATICLIB_WITH_ICU
    this->stringVal = new icu::UnicodeString(stringValue);
#else    
    this->stringVal = new std::string(stringValue);
#endif // STATICLIB_WITH_ICU    
}

JsonValue::JsonValue(int32_t integerValue) :
JsonValue(static_cast<int64_t>(integerValue)) { }

JsonValue::JsonValue(int64_t integerValue) :
type(JsonType::INTEGER), integerVal(integerValue) { }

JsonValue::JsonValue(uint32_t integerValue) :
JsonValue(static_cast<int64_t> (integerValue)) { }

JsonValue::JsonValue(double realValue) :
type(JsonType::REAL), realVal(realValue) { }

JsonValue::JsonValue(bool booleanValue) :
type(JsonType::BOOLEAN), booleanVal(booleanValue) { }

JsonValue JsonValue::clone() const {
    switch (type) {
    case JsonType::NULL_T: return JsonValue();
    case JsonType::OBJECT: {
        auto vec = std::vector<JsonField>{};
        vec.reserve(objectVal->size());
        for (const JsonField& fi : *objectVal) {
            vec.emplace_back(fi.get_name(), fi.get_value().clone());
        }
        return JsonValue(std::move(vec));
    }
    case JsonType::ARRAY: {
        auto vec = std::vector<JsonValue>{};
        vec.reserve(arrayVal->size());
        for (const JsonValue& va : *arrayVal) {
            vec.emplace_back(va.clone());
        }
        return JsonValue(std::move(vec));
    }
    case JsonType::STRING: return JsonValue(*stringVal);
    case JsonType::INTEGER: return JsonValue(integerVal);
    case JsonType::REAL: return JsonValue(realVal);
    case JsonType::BOOLEAN: return JsonValue(booleanVal);
    default: return JsonValue();
    }
}

JsonType JsonValue::get_type() const {
    return this->type;
}

const std::vector<JsonField>& JsonValue::get_object() const {
    if (JsonType::OBJECT == type) {
        return *(this->objectVal);
    } 
    return EMPTY_OBJECT;
}

std::pair<std::vector<JsonField>*, bool> JsonValue::get_object_ptr() {
    if (JsonType::OBJECT == type) {
        return {this->objectVal, true};
    }
    return {nullptr, false};
}

#ifdef STATICLIB_WITH_ICU
const JsonValue& JsonValue::get(const icu::UnicodeString& name) const {
    for (auto& el : this->get_object()) {
        if (name == el.get_name()) {
            return el.get_value();
        }
    }
    return NULL_VALUE;
}
#else
const JsonValue& JsonValue::get(const std::string& name) const {
    for (auto& el : this->get_object()) {
        if (name == el.get_name()) {
            return el.get_value();
        }
    }
    return NULL_VALUE;
}
#endif // STATICLIB_WITH_ICU

const std::vector<JsonValue>& JsonValue::get_array() const {
    if (JsonType::ARRAY == type) {
        return *(this->arrayVal);
    }
    return EMPTY_ARRAY;
}

std::pair<std::vector<JsonValue>*, bool> JsonValue::get_array_ptr() {
    if (JsonType::ARRAY == type) {
        return {this->arrayVal, true};
    }
    return {nullptr, false};
}

#ifdef STATICLIB_WITH_ICU
const icu::UnicodeString& JsonValue::get_string() const {
    if (JsonType::STRING == type) {
        return *(this->stringVal);
    }
    return EMPTY_STRING;
}

const icu::UnicodeString& JsonValue::get_string(const icu::UnicodeString& default_val) const {
    if (JsonType::STRING == type) {
        return *(this->stringVal);
    }
    return default_val;
}

bool JsonValue::set_string(icu::UnicodeString value) {
    if (JsonType::STRING == type) {
        *(this->stringVal) = std::move(value);
        return true;
    }
    return false;
}
#else
const std::string& JsonValue::get_string() const {
    if (JsonType::STRING == type) {
        return *(this->stringVal);
    }
    return EMPTY_STRING;
}

const std::string& JsonValue::get_string(const std::string& default_val) const {
    if (JsonType::STRING == type) {
        return *(this->stringVal);
    }
    return default_val;
}

bool JsonValue::set_string(std::string value) {
    if (JsonType::STRING == type) {
        *(this->stringVal) = std::move(value);
        return true;
    }
    return false;
}
#endif // STATICLIB_WITH_ICU

int64_t JsonValue::get_integer() const {
    if (JsonType::INTEGER == type) {
        return this->integerVal;
    }
    return 0;
}

int64_t JsonValue::get_integer(int64_t default_val) const {
    if (JsonType::INTEGER == type) {
        return this->integerVal;
    }
    return default_val;
}

bool JsonValue::set_integer(int64_t value) {
    if (JsonType::INTEGER == type) {
        this->integerVal = value;
        return true;
    }
    return false;
}

int32_t JsonValue::get_int32() const {
    if (JsonType::INTEGER == type) {
        return static_cast<int32_t> (this->integerVal);
    }
    return 0;
}

int32_t JsonValue::get_int32(int32_t default_val) const {
    if (JsonType::INTEGER == type) {
        return static_cast<int32_t> (this->integerVal);
    }
    return default_val;
}

bool JsonValue::set_int32(int32_t value) {
    if (JsonType::INTEGER == type) {
        this->integerVal = value;
        return true;
    }
    return false;
}

uint32_t JsonValue::get_uint32() const {
    if (JsonType::INTEGER == type) {
        return static_cast<uint32_t> (this->integerVal);
    }
    return 0;
}

uint32_t JsonValue::get_uint32(uint32_t default_val) const {
    if (JsonType::INTEGER == type) {
        return static_cast<uint32_t> (this->integerVal);
    }
    return default_val;
}

bool JsonValue::set_uint32(uint32_t value) {
    if (JsonType::INTEGER == type) {
        this->integerVal = value;
        return true;
    }
    return false;
}

int16_t JsonValue::get_int16() const {
    if (JsonType::INTEGER == type) {
        return static_cast<int16_t> (this->integerVal);
    }
    return 0;
}

int16_t JsonValue::get_int16(int16_t default_val) const {
    if (JsonType::INTEGER == type) {
        return static_cast<int16_t> (this->integerVal);
    }
    return default_val;
}

bool JsonValue::set_int16(int16_t value) {
    if (JsonType::INTEGER == type) {
        this->integerVal = value;
        return true;
    }
    return false;
}

uint16_t JsonValue::get_uint16() const {
    if (JsonType::INTEGER == type) {
        return static_cast<uint16_t> (this->integerVal);
    }
    return 0;
}

uint16_t JsonValue::get_uint16(uint16_t default_val) const {
    if (JsonType::INTEGER == type) {
        return static_cast<uint16_t> (this->integerVal);
    }
    return default_val;
}

bool JsonValue::set_uint16(uint16_t value) {
    if (JsonType::INTEGER == type) {
        this->integerVal = value;
        return true;
    }
    return false;
}

double JsonValue::get_real() const {
    if (JsonType::REAL == type) {
        return this->realVal;
    }
    return 0;
}

double JsonValue::get_real(double default_val) const {
    if (JsonType::REAL == type) {
        return this->realVal;
    }
    return default_val;
}

bool JsonValue::set_real(double value) {
    if (JsonType::REAL == type) {
        this->realVal = value;
        return true;
    }
    return false;    
}

bool JsonValue::get_boolean() const {
    if (JsonType::BOOLEAN == type) {
        return this->booleanVal;
    }
    return false;
}

bool JsonValue::get_boolean(bool default_val) const {
    if (JsonType::BOOLEAN == type) {
        return this->booleanVal;
    }
    return default_val;
}

bool JsonValue::set_boolean(bool value) {
    if (JsonType::BOOLEAN == type) {
        this->booleanVal = value;
        return true;
    }
    return false; 
}

}
} // namespace
