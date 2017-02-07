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
 * File:   json_value.cpp
 * Author: alex
 * 
 * Created on December 28, 2014, 8:38 PM
 */

#include "staticlib/serialization/json_value.hpp"

#ifdef STATICLIB_WITH_ICU
#include "staticlib/icu_utils.hpp"
#endif // STATICLIB_WITH_ICU

#include "staticlib/config.hpp"

#include "staticlib/serialization/json_field.hpp"
#include "staticlib/serialization/json_operations.hpp"

namespace staticlib {
namespace serialization {

namespace { // anonymous

namespace sc = staticlib::config;
#ifdef STATICLIB_WITH_ICU
namespace su = staticlib::icu_utils;
#endif // STATICLIB_WITH_ICU

const std::vector<json_field> EMPTY_OBJECT{};
const std::vector<json_value> EMPTY_ARRAY{};
const std::string EMPTY_STRING{};
const json_value NULL_VALUE{};

#ifdef STATICLIB_WITH_ICU
const icu::UnicodeString EMPTY_USTRING{};
#endif // STATICLIB_WITH_ICU

} // namespace

json_value::~json_value() STATICLIB_NOEXCEPT {
    switch (this->jsonType) {
    case json_type::NULL_T: break;
    case json_type::OBJECT: delete this->objectVal; break;
    case json_type::ARRAY: delete this->arrayVal; break;
    case json_type::STRING: delete this->stringVal; break;
    case json_type::INTEGER: break;
    case json_type::REAL: break;
    case json_type::BOOLEAN: break;
    }
}

json_value::json_value(json_value&& other) STATICLIB_NOEXCEPT {
    switch (other.jsonType) {
    case json_type::NULL_T: break;
    case json_type::OBJECT: this->objectVal = other.objectVal; break;
    case json_type::ARRAY: this->arrayVal = other.arrayVal; break;
    case json_type::STRING: this->stringVal = other.stringVal; break;
    case json_type::INTEGER: this->integerVal = other.integerVal; break;
    case json_type::REAL: this->realVal = other.realVal; break;
    case json_type::BOOLEAN: this->booleanVal = other.booleanVal; break;
    }
    this->jsonType = other.jsonType;
    // moved from object is empty now
    other.jsonType = json_type::NULL_T;
}

json_value& json_value::operator=(json_value&& other) STATICLIB_NOEXCEPT {
    // destroy existing value
    switch (this->jsonType) {
    case json_type::NULL_T: break;
    case json_type::OBJECT: delete this->objectVal; break;
    case json_type::ARRAY: delete this->arrayVal; break;
    case json_type::STRING: delete this->stringVal; break;
    case json_type::INTEGER: break;
    case json_type::REAL: break;
    case json_type::BOOLEAN: break;
    }
    // assign new value
    switch (other.jsonType) {
    case json_type::NULL_T: this->nullVal = other.nullVal; break;
    case json_type::OBJECT: this->objectVal = other.objectVal; break;
    case json_type::ARRAY: this->arrayVal = other.arrayVal; break;
    case json_type::STRING: this->stringVal = other.stringVal; break;
    case json_type::INTEGER: this->integerVal = other.integerVal; break;
    case json_type::REAL: this->realVal = other.realVal; break;
    case json_type::BOOLEAN: this->booleanVal = other.booleanVal; break;
    }
    this->jsonType = other.jsonType;
    // moved from object is empty now
    other.jsonType = json_type::NULL_T;
    return *this;
}

json_value::json_value() :
jsonType(json_type::NULL_T) { }

json_value::json_value(std::nullptr_t nullValue) :
jsonType(json_type::NULL_T) { (void) nullValue; }

json_value::json_value(std::vector<json_field>&& objectValue) :
jsonType(json_type::OBJECT) { 
    // allocate empty vector and move data into it
    this->objectVal = new std::vector<json_field>{};
    *(this->objectVal) = std::move(objectValue);
}

json_value::json_value(const std::initializer_list<json_field>& objectValue) :
jsonType(json_type::OBJECT) {
    this->objectVal = new std::vector<json_field>{};
    for (auto& a : objectValue) {
        this->objectVal->emplace_back(a.name(), a.value().clone());
    }
}

json_value::json_value(std::vector<json_value>&& arrayValue) :
jsonType(json_type::ARRAY) { 
    // allocate empty vector and move data into it
    this->arrayVal = new std::vector<json_value>{};
    *(this->arrayVal) = std::move(arrayValue);
}

json_value::json_value(const std::string& stringValue) :
jsonType(json_type::STRING) {
    std::string copy(stringValue.data(), stringValue.length());
    this->stringVal = new std::string();
    *(this->stringVal) = std::move(copy);
}

json_value::json_value(std::string&& stringValue) :
jsonType(json_type::STRING) {
    this->stringVal = new std::string();
    *(this->stringVal) = std::move(stringValue);
}

#ifdef STATICLIB_WITH_ICU
json_value::json_value(icu::UnicodeString ustringValue) :
json_value(su::to_utf8(ustringValue)) { }
#endif // STATICLIB_WITH_ICU


json_value::json_value(const char* stringValue) :
jsonType(json_type::STRING) {
    this->stringVal = new std::string(stringValue);
}

json_value::json_value(int32_t integerValue) :
json_value(static_cast<int64_t>(integerValue)) { }

json_value::json_value(int64_t integerValue) :
jsonType(json_type::INTEGER), integerVal(integerValue) { }

json_value::json_value(uint32_t integerValue) :
json_value(static_cast<int64_t> (integerValue)) { }

json_value::json_value(int16_t integerValue) :
json_value(static_cast<int64_t> (integerValue)) { }

json_value::json_value(uint16_t integerValue) :
json_value(static_cast<int64_t> (integerValue)) { }

json_value::json_value(double realValue) :
jsonType(json_type::REAL), realVal(realValue) { }

json_value::json_value(float realValue) :
json_value(static_cast<double> (realValue)) { }

json_value::json_value(bool booleanValue) :
jsonType(json_type::BOOLEAN), booleanVal(booleanValue) { }

json_value json_value::clone() const {
    switch (jsonType) {
    case json_type::NULL_T: return json_value();
    case json_type::OBJECT: {
        auto vec = std::vector<json_field>{};
        vec.reserve(objectVal->size());
        for (const json_field& fi : *objectVal) {
            vec.emplace_back(fi.name(), fi.value().clone());
        }
        return json_value(std::move(vec));
    }
    case json_type::ARRAY: {
        auto vec = std::vector<json_value>{};
        vec.reserve(arrayVal->size());
        for (const json_value& va : *arrayVal) {
            vec.emplace_back(va.clone());
        }
        return json_value(std::move(vec));
    }
    case json_type::STRING: return json_value(*stringVal);
    case json_type::INTEGER: return json_value(integerVal);
    case json_type::REAL: return json_value(realVal);
    case json_type::BOOLEAN: return json_value(booleanVal);
    default: return json_value();
    }
}

json_type json_value::type() const {
    return this->jsonType;
}

const json_value& json_value::getattr(const std::string& name) const {
    for (auto& el : this->as_object()) {
        if (name == el.name()) {
            return el.value();
        }
    }
    return NULL_VALUE;
}

const json_value& json_value::operator[](const std::string& name) const {
    return this->getattr(name);
}

#ifdef STATICLIB_WITH_ICU
const json_value& json_value::getattru(const icu::UnicodeString& uname) const {
    for (auto& el : this->as_object()) {
        if (uname == el.uname()) {
            return el.value();
        }
    }
    return NULL_VALUE;
}
#endif // STATICLIB_WITH_ICU

json_value& json_value::getattr_or_throw(const std::string& name, const std::string& context) {
    if (json_type::OBJECT == jsonType) {
        std::vector<json_field>& obj = this->as_object_or_throw();
        for (json_field& el : obj) {
            if (name == el.name()) {
                return el.value();
            }
        }
        // add attr
        obj.emplace_back(json_field(name, json_value()));
        return obj[obj.size() - 1].value();
    }
    // not object    
    throw serialization_exception(TRACEMSG("Cannot get attribute: [" + name + "]" +
            " from target value: [" + dump_json_to_string(*this) + "],"
            " context: [" + context + "]"));
}

#ifdef STATICLIB_WITH_ICU

json_value& json_value::getattru_or_throw(const icu::UnicodeString& name, const icu::UnicodeString& context) {
    if (json_type::OBJECT == jsonType) {
        std::vector<json_field>& obj = this->as_object_or_throw();
        for (json_field& el : obj) {
            if (name == el.uname()) {
                return el.value();
            }
        }
        // add attr
        obj.emplace_back(json_field(name, json_value()));
        return obj[obj.size() - 1].value();
    }
    // not object    
    throw serialization_exception(TRACEMSG("Cannot get attribute: [" + su::to_utf8(name) + "]" +
            " from target value: [" + dump_json_to_string(*this) + "],"
            " context: [" + su::to_utf8(context) + "]"));
}

#endif // STATICLIB_WITH_ICU  

const std::vector<json_field>& json_value::as_object() const {
    if (json_type::OBJECT == jsonType) {
        return *(this->objectVal);
    }
    return EMPTY_OBJECT;
}

std::vector<json_field>& json_value::as_object_or_throw(const std::string& context) {
    return const_cast<std::vector<json_field>&> (const_cast<const json_value*> (this)->as_object_or_throw(context));
}

const std::vector<json_field>& json_value::as_object_or_throw(const std::string& context) const {
    if (json_type::OBJECT == jsonType) {
        return *(this->objectVal);
    }
    // not object    
    throw serialization_exception(TRACEMSG("Cannot access object" +
            " from target value: [" + dump_json_to_string(*this) + "],"
            " context: [" + context + "]"));
}

bool json_value::set_object(std::vector<json_field>&& value) {
    if (json_type::OBJECT == jsonType) {
        *(this->objectVal) = std::move(value);
        return true;
    }
    *this = json_value(std::move(value));
    return false;
}

const std::vector<json_value>& json_value::as_array() const {
    if (json_type::ARRAY == jsonType) {
        return *(this->arrayVal);
    }
    return EMPTY_ARRAY;
}

std::vector<json_value>& json_value::as_array_or_throw(const std::string& context) {
    return const_cast<std::vector<json_value>&> (const_cast<const json_value*> (this)->as_array_or_throw(context));
}

const std::vector<json_value>& json_value::as_array_or_throw(const std::string& context) const {
    if (json_type::ARRAY == jsonType) {
        return *(this->arrayVal);
    }
    // not array    
    throw serialization_exception(TRACEMSG("Cannot access array" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

bool json_value::set_array(std::vector<json_value>&& value) {
    if (json_type::ARRAY == jsonType) {
        *(this->arrayVal) = std::move(value);
        return true;
    }
    *this = json_value(std::move(value));
    return false;
}

const std::string& json_value::as_string() const {
    if (json_type::STRING == jsonType) {
        return *(this->stringVal);
    }
    return EMPTY_STRING;
}

std::string& json_value::as_string_or_throw(const std::string& context) {
    return const_cast<std::string&> (const_cast<const json_value*> (this)->as_string_or_throw(context));
}

const std::string& json_value::as_string_or_throw(const std::string& context) const {
    if (json_type::STRING == jsonType) {
        return *(this->stringVal);
    }
    // not string    
    throw serialization_exception(TRACEMSG("Cannot access string" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

const std::string& json_value::as_string(const std::string& default_val) const {
    if (json_type::STRING == jsonType) {
        return *(this->stringVal);
    }
    return default_val;
}

bool json_value::set_string(std::string value) {
    if (json_type::STRING == jsonType) {
        *(this->stringVal) = std::move(value);
        return true;
    }
    *this = json_value(std::move(value));
    return false;
}

#ifdef STATICLIB_WITH_ICU
const icu::UnicodeString& json_value::as_ustring() const {
    if (json_type::STRING == jsonType) {
        if (nullptr == ustringValCached.get()) {
            this->ustringValCached = std::unique_ptr<icu::UnicodeString>{new icu::UnicodeString{}};
            *(this->ustringValCached) = icu::UnicodeString::fromUTF8(*(this->stringVal));
        }
        return *(this->ustringValCached);
    }
    return EMPTY_USTRING;
}

const icu::UnicodeString& json_value::as_ustring_or_throw(const icu::UnicodeString& context) const {
    if (json_type::STRING == jsonType) {
        if (nullptr == ustringValCached.get()) {
            this->ustringValCached = std::unique_ptr<icu::UnicodeString>{new icu::UnicodeString{}};
            *(this->ustringValCached) = icu::UnicodeString::fromUTF8(*(this->stringVal));
        }
        return *(this->ustringValCached);
    }
    // not string    
    throw serialization_exception(TRACEMSG("Cannot access string" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + su::to_utf8(context) + "]"));
}

const icu::UnicodeString& json_value::as_ustring(const icu::UnicodeString& default_val) const {
    if (json_type::STRING == jsonType) {
        return as_ustring();
    }
    return default_val;
}

bool json_value::set_ustring(icu::UnicodeString value) {
    if (json_type::STRING == jsonType) {
        *(this->stringVal) = su::to_utf8(value);
        if (nullptr != ustringValCached.get()) {
            *ustringValCached = icu::UnicodeString::fromUTF8(*(this->stringVal));
        }
        return true;
    }
    *this = json_value(std::move(value));
    return false;
}
#endif // STATICLIB_WITH_ICU

int64_t json_value::as_int64() const {
    if (json_type::INTEGER == jsonType) {
        return this->integerVal;
    }
    return 0;
}

int64_t json_value::as_int64_or_throw(const std::string& context) const {
    if (json_type::INTEGER == jsonType) {
        return this->integerVal;
    }
    // not integer
    throw serialization_exception(TRACEMSG("Cannot access 'int64'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

int64_t json_value::as_int64(int64_t default_val) const {
    if (json_type::INTEGER == jsonType) {
        return this->integerVal;
    }
    return default_val;
}

bool json_value::set_int64(int64_t value) {
    if (json_type::INTEGER == jsonType) {
        this->integerVal = value;
        return true;
    }
    *this = json_value(value);
    return false;
}

int32_t json_value::as_int32() const {
    if (json_type::INTEGER == jsonType) {
        return static_cast<int32_t> (this->integerVal);
    }
    return 0;
}

int32_t json_value::as_int32_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sc::is_int32(val)) {
        return static_cast<int32_t>(val);
    }
    // not int32_t
    throw serialization_exception(TRACEMSG("Cannot access 'int32'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

int32_t json_value::as_int32(int32_t default_val) const {
    if (json_type::INTEGER == jsonType) {
        return static_cast<int32_t> (this->integerVal);
    }
    return default_val;
}

bool json_value::set_int32(int32_t value) {
    if (json_type::INTEGER == jsonType) {
        this->integerVal = value;
        return true;
    }
    *this = json_value(value);
    return false;
}

uint32_t json_value::as_uint32() const {
    if (json_type::INTEGER == jsonType) {
        return static_cast<uint32_t> (this->integerVal);
    }
    return 0;
}

uint32_t json_value::as_uint32_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sc::is_uint32(val)) {
        return static_cast<uint32_t> (val);
    }
    // not uint32_t
    throw serialization_exception(TRACEMSG("Cannot access 'uint32'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

uint32_t json_value::as_uint32_positive_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sc::is_uint32_positive(val)) {
        return static_cast<uint32_t> (val);
    }
    // not positive uint32_t
    throw serialization_exception(TRACEMSG("Cannot access positive 'uint32'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

uint32_t json_value::as_uint32(uint32_t default_val) const {
    if (json_type::INTEGER == jsonType) {
        return static_cast<uint32_t> (this->integerVal);
    }
    return default_val;
}

bool json_value::set_uint32(uint32_t value) {
    if (json_type::INTEGER == jsonType) {
        this->integerVal = value;
        return true;
    }
    *this = json_value(value);
    return false;
}

int16_t json_value::as_int16() const {
    if (json_type::INTEGER == jsonType) {
        return static_cast<int16_t> (this->integerVal);
    }
    return 0;
}

int16_t json_value::as_int16_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sc::is_int16(val)) {
        return static_cast<int16_t> (val);
    }
    // not int16_t
    throw serialization_exception(TRACEMSG("Cannot access 'int16'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

int16_t json_value::as_int16(int16_t default_val) const {
    if (json_type::INTEGER == jsonType) {
        return static_cast<int16_t> (this->integerVal);
    }
    return default_val;
}

bool json_value::set_int16(int16_t value) {
    if (json_type::INTEGER == jsonType) {
        this->integerVal = value;
        return true;
    }
    *this = json_value(value);
    return false;
}

uint16_t json_value::as_uint16() const {
    if (json_type::INTEGER == jsonType) {
        return static_cast<uint16_t> (this->integerVal);
    }
    return 0;
}

uint16_t json_value::as_uint16_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sc::is_uint16(val)) {
        return static_cast<uint16_t> (val);
    }
    // not uint16_t
    throw serialization_exception(TRACEMSG("Cannot access 'uint16'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

uint16_t json_value::as_uint16_positive_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sc::is_uint16_positive(val)) {
        return static_cast<int16_t> (val);
    }
    // not positive unt16_t
    throw serialization_exception(TRACEMSG("Cannot access 'int16'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

uint16_t json_value::as_uint16(uint16_t default_val) const {
    if (json_type::INTEGER == jsonType) {
        return static_cast<uint16_t> (this->integerVal);
    }
    return default_val;
}

bool json_value::set_uint16(uint16_t value) {
    if (json_type::INTEGER == jsonType) {
        this->integerVal = value;
        return true;
    }
    *this = json_value(value);
    return false;
}

double json_value::as_double() const {
    if (json_type::REAL == jsonType) {
        return this->realVal;
    }
    return 0;
}

double json_value::as_double_or_throw(const std::string& context) const {
    if (json_type::REAL == jsonType) {
        return this->realVal;
    }
    // not real
    throw serialization_exception(TRACEMSG("Cannot access 'double'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

double json_value::as_double(double default_val) const {
    if (json_type::REAL == jsonType) {
        return this->realVal;
    }
    return default_val;
}

bool json_value::set_double(double value) {
    if (json_type::REAL == jsonType) {
        this->realVal = value;
        return true;
    }
    *this = json_value(value);
    return false;    
}

float json_value::as_float() const {
    if (json_type::REAL == jsonType) {
        return static_cast<float>(this->realVal);
    }
    return 0;
}

float json_value::as_float_or_throw(const std::string& context) const {
    double val = this->as_double_or_throw(context);
    if (val >= std::numeric_limits<float>::min() && val <= std::numeric_limits<float>::max()) {
        return static_cast<float>(val);
    }
    // not float
    throw serialization_exception(TRACEMSG("Cannot access 'float'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

float json_value::as_float(float default_val) const {
    if (json_type::REAL == jsonType) {
        return static_cast<float> (this->realVal);
    }
    return default_val;
}

bool json_value::set_float(float value) {
    if (json_type::REAL == jsonType) {
        this->realVal = value;
        return true;
    }
    *this = json_value(value);
    return false;
}

bool json_value::as_bool() const {
    if (json_type::BOOLEAN == jsonType) {
        return this->booleanVal;
    }
    return false;
}

bool json_value::as_bool_or_throw(const std::string& context) const {
    if (json_type::BOOLEAN == jsonType) {
        return this->booleanVal;
    }
    // not boolean
    throw serialization_exception(TRACEMSG("Cannot access 'boolean'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

bool json_value::as_bool(bool default_val) const {
    if (json_type::BOOLEAN == jsonType) {
        return this->booleanVal;
    }
    return default_val;
}

bool json_value::set_bool(bool value) {
    if (json_type::BOOLEAN == jsonType) {
        this->booleanVal = value;
        return true;
    }
    *this = json_value(value);
    return false; 
}

}
} // namespace
