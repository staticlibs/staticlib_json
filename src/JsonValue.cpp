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

#ifdef STATICLIB_WITH_ICU
#include "staticlib/icu_utils.hpp"
#endif // STATICLIB_WITH_ICU

#include "staticlib/serialization/JsonField.hpp"
#include "staticlib/serialization/json.hpp"

namespace staticlib {
namespace serialization {

namespace { // anonymous

namespace sc = staticlib::config;
#ifdef STATICLIB_WITH_ICU
namespace su = staticlib::icu_utils;
#endif // STATICLIB_WITH_ICU

const std::vector<JsonField> EMPTY_OBJECT{};
const std::vector<JsonValue> EMPTY_ARRAY{};
const std::string EMPTY_STRING{};
const JsonValue NULL_VALUE{};

#ifdef STATICLIB_WITH_ICU
const icu::UnicodeString EMPTY_USTRING{};
#endif // STATICLIB_WITH_ICU

} // namespace

JsonValue::~JsonValue() STATICLIB_NOEXCEPT {
    switch (this->jsonType) {
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
    switch (other.jsonType) {
    case JsonType::NULL_T: break;
    case JsonType::OBJECT: this->objectVal = other.objectVal; break;
    case JsonType::ARRAY: this->arrayVal = other.arrayVal; break;
    case JsonType::STRING: this->stringVal = other.stringVal; break;
    case JsonType::INTEGER: this->integerVal = other.integerVal; break;
    case JsonType::REAL: this->realVal = other.realVal; break;
    case JsonType::BOOLEAN: this->booleanVal = other.booleanVal; break;
    }
    this->jsonType = other.jsonType;
    // moved from object is empty now
    other.jsonType = JsonType::NULL_T;
}

JsonValue& JsonValue::operator=(JsonValue&& other) STATICLIB_NOEXCEPT {
    // destroy existing value
    switch (this->jsonType) {
    case JsonType::NULL_T: break;
    case JsonType::OBJECT: delete this->objectVal; break;
    case JsonType::ARRAY: delete this->arrayVal; break;
    case JsonType::STRING: delete this->stringVal; break;
    case JsonType::INTEGER: break;
    case JsonType::REAL: break;
    case JsonType::BOOLEAN: break;
    }
    // assign new value
    switch (other.jsonType) {
    case JsonType::NULL_T: this->nullVal = other.nullVal; break;
    case JsonType::OBJECT: this->objectVal = other.objectVal; break;
    case JsonType::ARRAY: this->arrayVal = other.arrayVal; break;
    case JsonType::STRING: this->stringVal = other.stringVal; break;
    case JsonType::INTEGER: this->integerVal = other.integerVal; break;
    case JsonType::REAL: this->realVal = other.realVal; break;
    case JsonType::BOOLEAN: this->booleanVal = other.booleanVal; break;
    }
    this->jsonType = other.jsonType;
    // moved from object is empty now
    other.jsonType = JsonType::NULL_T;
    return *this;
}

JsonValue::JsonValue() :
jsonType(JsonType::NULL_T) { }

JsonValue::JsonValue(std::nullptr_t nullValue) :
jsonType(JsonType::NULL_T) { (void) nullValue; }

JsonValue::JsonValue(std::vector<JsonField>&& objectValue) :
jsonType(JsonType::OBJECT) { 
    // allocate empty vector and move data into it
    this->objectVal = new std::vector<JsonField>{};
    *(this->objectVal) = std::move(objectValue);
}

JsonValue::JsonValue(const std::initializer_list<JsonField>& objectValue) :
jsonType(JsonType::OBJECT) {
    this->objectVal = new std::vector<JsonField>{};
    for (auto& a : objectValue) {
        this->objectVal->emplace_back(a.name(), a.value().clone());
    }
}

JsonValue::JsonValue(std::vector<JsonValue>&& arrayValue) :
jsonType(JsonType::ARRAY) { 
    // allocate empty vector and move data into it
    this->arrayVal = new std::vector<JsonValue>{};
    *(this->arrayVal) = std::move(arrayValue);
}

JsonValue::JsonValue(const std::string& stringValue) :
jsonType(JsonType::STRING) {
    std::string copy(stringValue.data(), stringValue.length());
    this->stringVal = new std::string();
    *(this->stringVal) = std::move(copy);
}

JsonValue::JsonValue(std::string& stringValue) :
jsonType(JsonType::STRING) {
    std::string copy(stringValue.data(), stringValue.length());
    this->stringVal = new std::string();
    *(this->stringVal) = std::move(copy);
}

JsonValue::JsonValue(std::string&& stringValue) :
jsonType(JsonType::STRING) {
    this->stringVal = new std::string();
    *(this->stringVal) = std::move(stringValue);
}

#ifdef STATICLIB_WITH_ICU
JsonValue::JsonValue(icu::UnicodeString ustringValue) :
JsonValue(su::to_utf8(ustringValue)) { }
#endif // STATICLIB_WITH_ICU


JsonValue::JsonValue(const char* stringValue) :
jsonType(JsonType::STRING) {
    this->stringVal = new std::string(stringValue);
}

JsonValue::JsonValue(int32_t integerValue) :
JsonValue(static_cast<int64_t>(integerValue)) { }

JsonValue::JsonValue(int64_t integerValue) :
jsonType(JsonType::INTEGER), integerVal(integerValue) { }

JsonValue::JsonValue(uint32_t integerValue) :
JsonValue(static_cast<int64_t> (integerValue)) { }

JsonValue::JsonValue(int16_t integerValue) :
JsonValue(static_cast<int64_t> (integerValue)) { }

JsonValue::JsonValue(uint16_t integerValue) :
JsonValue(static_cast<int64_t> (integerValue)) { }

JsonValue::JsonValue(double realValue) :
jsonType(JsonType::REAL), realVal(realValue) { }

JsonValue::JsonValue(float realValue) :
JsonValue(static_cast<double> (realValue)) { }

JsonValue::JsonValue(bool booleanValue) :
jsonType(JsonType::BOOLEAN), booleanVal(booleanValue) { }

JsonValue JsonValue::clone() const {
    switch (jsonType) {
    case JsonType::NULL_T: return JsonValue();
    case JsonType::OBJECT: {
        auto vec = std::vector<JsonField>{};
        vec.reserve(objectVal->size());
        for (const JsonField& fi : *objectVal) {
            vec.emplace_back(fi.name(), fi.value().clone());
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

JsonType JsonValue::type() const {
    return this->jsonType;
}

const JsonValue& JsonValue::getattr(const std::string& name) const {
    for (auto& el : this->as_object()) {
        if (name == el.name()) {
            return el.value();
        }
    }
    return NULL_VALUE;
}

const JsonValue& JsonValue::operator[](const std::string& name) const {
    return this->getattr(name);
}

#ifdef STATICLIB_WITH_ICU
const JsonValue& JsonValue::getattru(const icu::UnicodeString& uname) const {
    for (auto& el : this->as_object()) {
        if (uname == el.uname()) {
            return el.value();
        }
    }
    return NULL_VALUE;
}
#endif // STATICLIB_WITH_ICU

JsonValue& JsonValue::getattr_or_throw(const std::string& name, const std::string& context) {
    if (JsonType::OBJECT == jsonType) {
        std::vector<JsonField>& obj = this->as_object_or_throw();
        for (JsonField& el : obj) {
            if (name == el.name()) {
                return el.value();
            }
        }
        // add attr
        obj.emplace_back(JsonField(name, JsonValue()));
        return obj[obj.size() - 1].value();
    }
    // not object    
    throw SerializationException(TRACEMSG("Cannot get attribute: [" + name + "]" +
            " from target value: [" + dump_json_to_string(*this) + "],"
            " context: [" + context + "]"));
}

#ifdef STATICLIB_WITH_ICU

JsonValue& JsonValue::getattru_mutable(const icu::UnicodeString& name) {
    auto pa = this->as_object_mutable();
    if (pa.second) { // actually object
        for (JsonField& el : *pa.first) {
            if (name == el.uname()) {
                return el.value();
            }
        }
        std::vector<JsonField>& obj = *pa.first;
        obj.emplace_back(JsonField(name, JsonValue()));
        return obj[obj.size() - 1].value();
    }
    // not object    
    *this = JsonValue(std::vector<JsonField>());
    std::vector<JsonField>& obj = *(this->as_object_mutable().first);
    obj.emplace_back(JsonField(name, JsonValue()));
    return obj[obj.size() - 1].value();
}

#endif // STATICLIB_WITH_ICU  

const std::vector<JsonField>& JsonValue::as_object() const {
    if (JsonType::OBJECT == jsonType) {
        return *(this->objectVal);
    }
    return EMPTY_OBJECT;
}

std::vector<JsonField>& JsonValue::as_object_or_throw(const std::string& context) {
    return const_cast<std::vector<JsonField>&> (const_cast<const JsonValue*> (this)->as_object_or_throw(context));
}

const std::vector<JsonField>& JsonValue::as_object_or_throw(const std::string& context) const {
    if (JsonType::OBJECT == jsonType) {
        return *(this->objectVal);
    }
    // not object    
    throw SerializationException(TRACEMSG("Cannot access object" +
            " from target value: [" + dump_json_to_string(*this) + "],"
            " context: [" + context + "]"));
}

bool JsonValue::set_object(std::vector<JsonField>&& value) {
    if (JsonType::OBJECT == jsonType) {
        *(this->objectVal) = std::move(value);
        return true;
    }
    *this = JsonValue(std::move(value));
    return false;
}

const std::vector<JsonValue>& JsonValue::as_array() const {
    if (JsonType::ARRAY == jsonType) {
        return *(this->arrayVal);
    }
    return EMPTY_ARRAY;
}

std::vector<JsonValue>& JsonValue::as_array_or_throw(const std::string& context) {
    return const_cast<std::vector<JsonValue>&> (const_cast<const JsonValue*> (this)->as_array_or_throw(context));
}

const std::vector<JsonValue>& JsonValue::as_array_or_throw(const std::string& context) const {
    if (JsonType::ARRAY == jsonType) {
        return *(this->arrayVal);
    }
    // not array    
    throw SerializationException(TRACEMSG("Cannot access array" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

bool JsonValue::set_array(std::vector<JsonValue>&& value) {
    if (JsonType::ARRAY == jsonType) {
        *(this->arrayVal) = std::move(value);
        return true;
    }
    *this = JsonValue(std::move(value));
    return false;
}

const std::string& JsonValue::as_string() const {
    if (JsonType::STRING == jsonType) {
        return *(this->stringVal);
    }
    return EMPTY_STRING;
}

std::string& JsonValue::as_string_or_throw(const std::string& context) {
    return const_cast<std::string&> (const_cast<const JsonValue*> (this)->as_string_or_throw(context));
}

const std::string& JsonValue::as_string_or_throw(const std::string& context) const {
    if (JsonType::STRING == jsonType) {
        return *(this->stringVal);
    }
    // not string    
    throw SerializationException(TRACEMSG("Cannot access string" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

const std::string& JsonValue::as_string(const std::string& default_val) const {
    if (JsonType::STRING == jsonType) {
        return *(this->stringVal);
    }
    return default_val;
}

bool JsonValue::set_string(std::string value) {
    if (JsonType::STRING == jsonType) {
        *(this->stringVal) = std::move(value);
        return true;
    }
    *this = JsonValue(std::move(value));
    return false;
}

#ifdef STATICLIB_WITH_ICU
const icu::UnicodeString& JsonValue::as_ustring() const {
    if (JsonType::STRING == jsonType) {
        if (nullptr == ustringValCached.get()) {
            this->ustringValCached = std::unique_ptr<icu::UnicodeString>{new icu::UnicodeString{}};
            *(this->ustringValCached) = icu::UnicodeString::fromUTF8(*(this->stringVal));
        }
        return *(this->ustringValCached);
    }
    return EMPTY_USTRING;
}

const icu::UnicodeString& JsonValue::as_ustring(const icu::UnicodeString& default_val) const {
    if (JsonType::STRING == jsonType) {
        return as_ustring();
    }
    return default_val;
}

bool JsonValue::set_ustring(icu::UnicodeString value) {
    if (JsonType::STRING == jsonType) {
        *(this->stringVal) = su::to_utf8(value);
        if (nullptr != ustringValCached.get()) {
            *ustringValCached = icu::UnicodeString::fromUTF8(*(this->stringVal));
        }
        return true;
    }
    *this = JsonValue(std::move(value));
    return false;
}
#endif // STATICLIB_WITH_ICU

int64_t JsonValue::as_int64() const {
    if (JsonType::INTEGER == jsonType) {
        return this->integerVal;
    }
    return 0;
}

int64_t JsonValue::as_int64_or_throw(const std::string& context) const {
    if (JsonType::INTEGER == jsonType) {
        return this->integerVal;
    }
    // not integer
    throw SerializationException(TRACEMSG("Cannot access 'int64'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

int64_t JsonValue::as_int64(int64_t default_val) const {
    if (JsonType::INTEGER == jsonType) {
        return this->integerVal;
    }
    return default_val;
}

bool JsonValue::set_int64(int64_t value) {
    if (JsonType::INTEGER == jsonType) {
        this->integerVal = value;
        return true;
    }
    *this = JsonValue(value);
    return false;
}

int32_t JsonValue::as_int32() const {
    if (JsonType::INTEGER == jsonType) {
        return static_cast<int32_t> (this->integerVal);
    }
    return 0;
}

int32_t JsonValue::as_int32_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sc::is_int32(val)) {
        return static_cast<int32_t>(val);
    }
    // not int32_t
    throw SerializationException(TRACEMSG("Cannot access 'int32'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

int32_t JsonValue::as_int32(int32_t default_val) const {
    if (JsonType::INTEGER == jsonType) {
        return static_cast<int32_t> (this->integerVal);
    }
    return default_val;
}

bool JsonValue::set_int32(int32_t value) {
    if (JsonType::INTEGER == jsonType) {
        this->integerVal = value;
        return true;
    }
    *this = JsonValue(value);
    return false;
}

uint32_t JsonValue::as_uint32() const {
    if (JsonType::INTEGER == jsonType) {
        return static_cast<uint32_t> (this->integerVal);
    }
    return 0;
}

uint32_t JsonValue::as_uint32_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sc::is_uint32(val)) {
        return static_cast<uint32_t> (val);
    }
    // not uint32_t
    throw SerializationException(TRACEMSG("Cannot access 'uint32'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

uint32_t JsonValue::as_uint32_positive_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sc::is_uint32_positive(val)) {
        return static_cast<uint32_t> (val);
    }
    // not positive uint32_t
    throw SerializationException(TRACEMSG("Cannot access positive 'uint32'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

uint32_t JsonValue::as_uint32(uint32_t default_val) const {
    if (JsonType::INTEGER == jsonType) {
        return static_cast<uint32_t> (this->integerVal);
    }
    return default_val;
}

bool JsonValue::set_uint32(uint32_t value) {
    if (JsonType::INTEGER == jsonType) {
        this->integerVal = value;
        return true;
    }
    *this = JsonValue(value);
    return false;
}

int16_t JsonValue::as_int16() const {
    if (JsonType::INTEGER == jsonType) {
        return static_cast<int16_t> (this->integerVal);
    }
    return 0;
}

int16_t JsonValue::as_int16_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sc::is_int16(val)) {
        return static_cast<int16_t> (val);
    }
    // not int16_t
    throw SerializationException(TRACEMSG("Cannot access 'int16'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

int16_t JsonValue::as_int16(int16_t default_val) const {
    if (JsonType::INTEGER == jsonType) {
        return static_cast<int16_t> (this->integerVal);
    }
    return default_val;
}

bool JsonValue::set_int16(int16_t value) {
    if (JsonType::INTEGER == jsonType) {
        this->integerVal = value;
        return true;
    }
    *this = JsonValue(value);
    return false;
}

uint16_t JsonValue::as_uint16() const {
    if (JsonType::INTEGER == jsonType) {
        return static_cast<uint16_t> (this->integerVal);
    }
    return 0;
}

uint16_t JsonValue::as_uint16_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sc::is_uint16(val)) {
        return static_cast<uint16_t> (val);
    }
    // not uint16_t
    throw SerializationException(TRACEMSG("Cannot access 'uint16'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

uint16_t JsonValue::as_uint16_positive_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sc::is_uint16_positive(val)) {
        return static_cast<int16_t> (val);
    }
    // not positive unt16_t
    throw SerializationException(TRACEMSG("Cannot access 'int16'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

uint16_t JsonValue::as_uint16(uint16_t default_val) const {
    if (JsonType::INTEGER == jsonType) {
        return static_cast<uint16_t> (this->integerVal);
    }
    return default_val;
}

bool JsonValue::set_uint16(uint16_t value) {
    if (JsonType::INTEGER == jsonType) {
        this->integerVal = value;
        return true;
    }
    *this = JsonValue(value);
    return false;
}

double JsonValue::as_double() const {
    if (JsonType::REAL == jsonType) {
        return this->realVal;
    }
    return 0;
}

double JsonValue::as_double_or_throw(const std::string& context) const {
    if (JsonType::REAL == jsonType) {
        return this->realVal;
    }
    // not real
    throw SerializationException(TRACEMSG("Cannot access 'double'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

double JsonValue::as_double(double default_val) const {
    if (JsonType::REAL == jsonType) {
        return this->realVal;
    }
    return default_val;
}

bool JsonValue::set_double(double value) {
    if (JsonType::REAL == jsonType) {
        this->realVal = value;
        return true;
    }
    *this = JsonValue(value);
    return false;    
}

float JsonValue::as_float() const {
    if (JsonType::REAL == jsonType) {
        return static_cast<float>(this->realVal);
    }
    return 0;
}

float JsonValue::as_float_or_throw(const std::string& context) const {
    double val = this->as_double_or_throw(context);
    if (val >= std::numeric_limits<float>::min() && val <= std::numeric_limits<float>::max()) {
        return static_cast<float>(val);
    }
    // not float
    throw SerializationException(TRACEMSG("Cannot access 'float'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

float JsonValue::as_float(float default_val) const {
    if (JsonType::REAL == jsonType) {
        return static_cast<float> (this->realVal);
    }
    return default_val;
}

bool JsonValue::set_float(float value) {
    if (JsonType::REAL == jsonType) {
        this->realVal = value;
        return true;
    }
    *this = JsonValue(value);
    return false;
}

bool JsonValue::as_bool() const {
    if (JsonType::BOOLEAN == jsonType) {
        return this->booleanVal;
    }
    return false;
}

bool JsonValue::as_bool_or_throw(const std::string& context) const {
    if (JsonType::BOOLEAN == jsonType) {
        return this->booleanVal;
    }
    // not boolean
    throw SerializationException(TRACEMSG("Cannot access 'boolean'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

bool JsonValue::as_bool(bool default_val) const {
    if (JsonType::BOOLEAN == jsonType) {
        return this->booleanVal;
    }
    return default_val;
}

bool JsonValue::set_bool(bool value) {
    if (JsonType::BOOLEAN == jsonType) {
        this->booleanVal = value;
        return true;
    }
    *this = JsonValue(value);
    return false; 
}

}
} // namespace
