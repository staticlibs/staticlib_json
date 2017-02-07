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
    switch (this->value_type) {
    case json_type::nullt: break;
    case json_type::object: delete this->object_val; break;
    case json_type::array: delete this->array_val; break;
    case json_type::string: delete this->string_val; break;
    case json_type::integer: break;
    case json_type::real: break;
    case json_type::boolean: break;
    }
}

json_value::json_value(json_value&& other) STATICLIB_NOEXCEPT {
    switch (other.value_type) {
    case json_type::nullt: break;
    case json_type::object: this->object_val = other.object_val; break;
    case json_type::array: this->array_val = other.array_val; break;
    case json_type::string: this->string_val = other.string_val; break;
    case json_type::integer: this->integer_val = other.integer_val; break;
    case json_type::real: this->real_val = other.real_val; break;
    case json_type::boolean: this->boolean_val = other.boolean_val; break;
    }
    this->value_type = other.value_type;
    // moved from object is empty now
    other.value_type = json_type::nullt;
}

json_value& json_value::operator=(json_value&& other) STATICLIB_NOEXCEPT {
    // destroy existing value
    switch (this->value_type) {
    case json_type::nullt: break;
    case json_type::object: delete this->object_val; break;
    case json_type::array: delete this->array_val; break;
    case json_type::string: delete this->string_val; break;
    case json_type::integer: break;
    case json_type::real: break;
    case json_type::boolean: break;
    }
    // assign new value
    switch (other.value_type) {
    case json_type::nullt: this->null_val = other.null_val; break;
    case json_type::object: this->object_val = other.object_val; break;
    case json_type::array: this->array_val = other.array_val; break;
    case json_type::string: this->string_val = other.string_val; break;
    case json_type::integer: this->integer_val = other.integer_val; break;
    case json_type::real: this->real_val = other.real_val; break;
    case json_type::boolean: this->boolean_val = other.boolean_val; break;
    }
    this->value_type = other.value_type;
    // moved from object is empty now
    other.value_type = json_type::nullt;
    return *this;
}

json_value::json_value() :
value_type(json_type::nullt) { }

json_value::json_value(std::nullptr_t null_value) :
value_type(json_type::nullt) { (void) null_value; }

json_value::json_value(std::vector<json_field>&& object_value) :
value_type(json_type::object) { 
    // allocate empty vector and move data into it
    this->object_val = new std::vector<json_field>{};
    *(this->object_val) = std::move(object_value);
}

json_value::json_value(const std::initializer_list<json_field>& object_value) :
value_type(json_type::object) {
    this->object_val = new std::vector<json_field>{};
    for (auto& a : object_value) {
        this->object_val->emplace_back(a.name(), a.value().clone());
    }
}

json_value::json_value(std::vector<json_value>&& array_value) :
value_type(json_type::array) { 
    // allocate empty vector and move data into it
    this->array_val = new std::vector<json_value>{};
    *(this->array_val) = std::move(array_value);
}

json_value::json_value(const std::string& string_value) :
value_type(json_type::string) {
    std::string copy(string_value.data(), string_value.length());
    this->string_val = new std::string();
    *(this->string_val) = std::move(copy);
}

json_value::json_value(std::string&& string_value) :
value_type(json_type::string) {
    this->string_val = new std::string();
    *(this->string_val) = std::move(string_value);
}

#ifdef STATICLIB_WITH_ICU
json_value::json_value(icu::UnicodeString ustring_value) :
json_value(su::to_utf8(ustring_value)) { }
#endif // STATICLIB_WITH_ICU


json_value::json_value(const char* string_value) :
value_type(json_type::string) {
    this->string_val = new std::string(string_value);
}

json_value::json_value(int32_t integer_value) :
json_value(static_cast<int64_t>(integer_value)) { }

json_value::json_value(int64_t integer_value) :
value_type(json_type::integer), integer_val(integer_value) { }

json_value::json_value(uint32_t integer_value) :
json_value(static_cast<int64_t> (integer_value)) { }

json_value::json_value(int16_t integer_value) :
json_value(static_cast<int64_t> (integer_value)) { }

json_value::json_value(uint16_t integer_value) :
json_value(static_cast<int64_t> (integer_value)) { }

json_value::json_value(double real_value) :
value_type(json_type::real), real_val(real_value) { }

json_value::json_value(float real_value) :
json_value(static_cast<double> (real_value)) { }

json_value::json_value(bool boolean_value) :
value_type(json_type::boolean), boolean_val(boolean_value) { }

json_value json_value::clone() const {
    switch (value_type) {
    case json_type::nullt: return json_value();
    case json_type::object: {
        auto vec = std::vector<json_field>{};
        vec.reserve(object_val->size());
        for (const json_field& fi : *object_val) {
            vec.emplace_back(fi.name(), fi.value().clone());
        }
        return json_value(std::move(vec));
    }
    case json_type::array: {
        auto vec = std::vector<json_value>{};
        vec.reserve(array_val->size());
        for (const json_value& va : *array_val) {
            vec.emplace_back(va.clone());
        }
        return json_value(std::move(vec));
    }
    case json_type::string: return json_value(*string_val);
    case json_type::integer: return json_value(integer_val);
    case json_type::real: return json_value(real_val);
    case json_type::boolean: return json_value(boolean_val);
    default: return json_value();
    }
}

json_type json_value::type() const {
    return this->value_type;
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
    if (json_type::object == value_type) {
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
    if (json_type::object == value_type) {
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
    if (json_type::object == value_type) {
        return *(this->object_val);
    }
    return EMPTY_OBJECT;
}

std::vector<json_field>& json_value::as_object_or_throw(const std::string& context) {
    return const_cast<std::vector<json_field>&> (const_cast<const json_value*> (this)->as_object_or_throw(context));
}

const std::vector<json_field>& json_value::as_object_or_throw(const std::string& context) const {
    if (json_type::object == value_type) {
        return *(this->object_val);
    }
    // not object    
    throw serialization_exception(TRACEMSG("Cannot access object" +
            " from target value: [" + dump_json_to_string(*this) + "],"
            " context: [" + context + "]"));
}

bool json_value::set_object(std::vector<json_field>&& value) {
    if (json_type::object == value_type) {
        *(this->object_val) = std::move(value);
        return true;
    }
    *this = json_value(std::move(value));
    return false;
}

const std::vector<json_value>& json_value::as_array() const {
    if (json_type::array == value_type) {
        return *(this->array_val);
    }
    return EMPTY_ARRAY;
}

std::vector<json_value>& json_value::as_array_or_throw(const std::string& context) {
    return const_cast<std::vector<json_value>&> (const_cast<const json_value*> (this)->as_array_or_throw(context));
}

const std::vector<json_value>& json_value::as_array_or_throw(const std::string& context) const {
    if (json_type::array == value_type) {
        return *(this->array_val);
    }
    // not array    
    throw serialization_exception(TRACEMSG("Cannot access array" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

bool json_value::set_array(std::vector<json_value>&& value) {
    if (json_type::array == value_type) {
        *(this->array_val) = std::move(value);
        return true;
    }
    *this = json_value(std::move(value));
    return false;
}

const std::string& json_value::as_string() const {
    if (json_type::string == value_type) {
        return *(this->string_val);
    }
    return EMPTY_STRING;
}

std::string& json_value::as_string_or_throw(const std::string& context) {
    return const_cast<std::string&> (const_cast<const json_value*> (this)->as_string_or_throw(context));
}

const std::string& json_value::as_string_or_throw(const std::string& context) const {
    if (json_type::string == value_type) {
        return *(this->string_val);
    }
    // not string    
    throw serialization_exception(TRACEMSG("Cannot access string" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

const std::string& json_value::as_string(const std::string& default_val) const {
    if (json_type::string == value_type) {
        return *(this->string_val);
    }
    return default_val;
}

bool json_value::set_string(std::string value) {
    if (json_type::string == value_type) {
        *(this->string_val) = std::move(value);
        return true;
    }
    *this = json_value(std::move(value));
    return false;
}

#ifdef STATICLIB_WITH_ICU
const icu::UnicodeString& json_value::as_ustring() const {
    if (json_type::string == value_type) {
        if (nullptr == ustring_val_cached.get()) {
            this->ustring_val_cached = std::unique_ptr<icu::UnicodeString>{new icu::UnicodeString{}};
            *(this->ustring_val_cached) = icu::UnicodeString::fromUTF8(*(this->string_val));
        }
        return *(this->ustring_val_cached);
    }
    return EMPTY_USTRING;
}

const icu::UnicodeString& json_value::as_ustring_or_throw(const icu::UnicodeString& context) const {
    if (json_type::string == value_type) {
        if (nullptr == ustring_val_cached.get()) {
            this->ustring_val_cached = std::unique_ptr<icu::UnicodeString>{new icu::UnicodeString{}};
            *(this->ustring_val_cached) = icu::UnicodeString::fromUTF8(*(this->string_val));
        }
        return *(this->ustring_val_cached);
    }
    // not string    
    throw serialization_exception(TRACEMSG("Cannot access string" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + su::to_utf8(context) + "]"));
}

const icu::UnicodeString& json_value::as_ustring(const icu::UnicodeString& default_val) const {
    if (json_type::string == value_type) {
        return as_ustring();
    }
    return default_val;
}

bool json_value::set_ustring(icu::UnicodeString value) {
    if (json_type::string == value_type) {
        *(this->string_val) = su::to_utf8(value);
        if (nullptr != ustring_val_cached.get()) {
            *ustring_val_cached = icu::UnicodeString::fromUTF8(*(this->string_val));
        }
        return true;
    }
    *this = json_value(std::move(value));
    return false;
}
#endif // STATICLIB_WITH_ICU

int64_t json_value::as_int64() const {
    if (json_type::integer == value_type) {
        return this->integer_val;
    }
    return 0;
}

int64_t json_value::as_int64_or_throw(const std::string& context) const {
    if (json_type::integer == value_type) {
        return this->integer_val;
    }
    // not integer
    throw serialization_exception(TRACEMSG("Cannot access 'int64'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

int64_t json_value::as_int64(int64_t default_val) const {
    if (json_type::integer == value_type) {
        return this->integer_val;
    }
    return default_val;
}

bool json_value::set_int64(int64_t value) {
    if (json_type::integer == value_type) {
        this->integer_val = value;
        return true;
    }
    *this = json_value(value);
    return false;
}

int32_t json_value::as_int32() const {
    if (json_type::integer == value_type) {
        return static_cast<int32_t> (this->integer_val);
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
    if (json_type::integer == value_type) {
        return static_cast<int32_t> (this->integer_val);
    }
    return default_val;
}

bool json_value::set_int32(int32_t value) {
    if (json_type::integer == value_type) {
        this->integer_val = value;
        return true;
    }
    *this = json_value(value);
    return false;
}

uint32_t json_value::as_uint32() const {
    if (json_type::integer == value_type) {
        return static_cast<uint32_t> (this->integer_val);
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
    if (json_type::integer == value_type) {
        return static_cast<uint32_t> (this->integer_val);
    }
    return default_val;
}

bool json_value::set_uint32(uint32_t value) {
    if (json_type::integer == value_type) {
        this->integer_val = value;
        return true;
    }
    *this = json_value(value);
    return false;
}

int16_t json_value::as_int16() const {
    if (json_type::integer == value_type) {
        return static_cast<int16_t> (this->integer_val);
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
    if (json_type::integer == value_type) {
        return static_cast<int16_t> (this->integer_val);
    }
    return default_val;
}

bool json_value::set_int16(int16_t value) {
    if (json_type::integer == value_type) {
        this->integer_val = value;
        return true;
    }
    *this = json_value(value);
    return false;
}

uint16_t json_value::as_uint16() const {
    if (json_type::integer == value_type) {
        return static_cast<uint16_t> (this->integer_val);
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
    if (json_type::integer == value_type) {
        return static_cast<uint16_t> (this->integer_val);
    }
    return default_val;
}

bool json_value::set_uint16(uint16_t value) {
    if (json_type::integer == value_type) {
        this->integer_val = value;
        return true;
    }
    *this = json_value(value);
    return false;
}

double json_value::as_double() const {
    if (json_type::real == value_type) {
        return this->real_val;
    }
    return 0;
}

double json_value::as_double_or_throw(const std::string& context) const {
    if (json_type::real == value_type) {
        return this->real_val;
    }
    // not real
    throw serialization_exception(TRACEMSG("Cannot access 'double'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

double json_value::as_double(double default_val) const {
    if (json_type::real == value_type) {
        return this->real_val;
    }
    return default_val;
}

bool json_value::set_double(double value) {
    if (json_type::real == value_type) {
        this->real_val = value;
        return true;
    }
    *this = json_value(value);
    return false;    
}

float json_value::as_float() const {
    if (json_type::real == value_type) {
        return static_cast<float>(this->real_val);
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
    if (json_type::real == value_type) {
        return static_cast<float> (this->real_val);
    }
    return default_val;
}

bool json_value::set_float(float value) {
    if (json_type::real == value_type) {
        this->real_val = value;
        return true;
    }
    *this = json_value(value);
    return false;
}

bool json_value::as_bool() const {
    if (json_type::boolean == value_type) {
        return this->boolean_val;
    }
    return false;
}

bool json_value::as_bool_or_throw(const std::string& context) const {
    if (json_type::boolean == value_type) {
        return this->boolean_val;
    }
    // not boolean
    throw serialization_exception(TRACEMSG("Cannot access 'boolean'" +
            " from target value: [" + dump_json_to_string(*this) + "]," +
            " context: [" + context + "]"));
}

bool json_value::as_bool(bool default_val) const {
    if (json_type::boolean == value_type) {
        return this->boolean_val;
    }
    return default_val;
}

bool json_value::set_bool(bool value) {
    if (json_type::boolean == value_type) {
        this->boolean_val = value;
        return true;
    }
    *this = json_value(value);
    return false; 
}

}
} // namespace
