/* 
 * File:   value.cpp
 * Author: alex
 *
 * Created on April 9, 2017, 5:54 PM
 */

#include "staticlib/json/value.hpp"

#include "staticlib/config.hpp"

#include "staticlib/json/field.hpp"

#include "jansson_ops.hpp"

namespace staticlib {
namespace json {

namespace { // anonymous

const std::vector<field> empty_object{};
const std::vector<value> empty_array{};
const std::string empty_string{};
const value null_value{};

} // namespace

value::~value() STATICLIB_NOEXCEPT {
    switch (this->value_type) {
    case type::nullt: break;
    case type::object: delete this->object_val;
        break;
    case type::array: delete this->array_val;
        break;
    case type::string: delete this->string_val;
        break;
    case type::integer: break;
    case type::real: break;
    case type::boolean: break;
    }
}

value::value(value&& other) STATICLIB_NOEXCEPT {
    switch (other.value_type) {
    case type::nullt: break;
    case type::object: this->object_val = other.object_val;
        break;
    case type::array: this->array_val = other.array_val;
        break;
    case type::string: this->string_val = other.string_val;
        break;
    case type::integer: this->integer_val = other.integer_val;
        break;
    case type::real: this->real_val = other.real_val;
        break;
    case type::boolean: this->boolean_val = other.boolean_val;
        break;
    }
    this->value_type = other.value_type;
    // moved from object is empty now
    other.value_type = type::nullt;
}

value& value::operator=(value&& other) STATICLIB_NOEXCEPT {
    // destroy existing value
    switch (this->value_type) {
    case type::nullt: break;
    case type::object: delete this->object_val;
        break;
    case type::array: delete this->array_val;
        break;
    case type::string: delete this->string_val;
        break;
    case type::integer: break;
    case type::real: break;
    case type::boolean: break;
    }
    // assign new value
    switch (other.value_type) {
    case type::nullt: this->null_val = other.null_val;
        break;
    case type::object: this->object_val = other.object_val;
        break;
    case type::array: this->array_val = other.array_val;
        break;
    case type::string: this->string_val = other.string_val;
        break;
    case type::integer: this->integer_val = other.integer_val;
        break;
    case type::real: this->real_val = other.real_val;
        break;
    case type::boolean: this->boolean_val = other.boolean_val;
        break;
    }
    this->value_type = other.value_type;
    // moved from object is empty now
    other.value_type = type::nullt;
    return *this;
}

value::value() :
value_type(type::nullt) { }

value::value(std::nullptr_t null_value) :
value_type(type::nullt) {
    (void) null_value;
}

value::value(std::vector<field>&& object_value) :
value_type(type::object) {
    // allocate empty vector and move data into it
    this->object_val = new std::vector<field>();
    *(this->object_val) = std::move(object_value);
}

value::value(const std::initializer_list<field>& object_value) :
value_type(type::object) {
    this->object_val = new std::vector<field>();
    for (auto& a : object_value) {
        this->object_val->emplace_back(a.name(), a.val().clone());
    }
}

value::value(std::vector<value>&& array_value) :
value_type(type::array) {
    // allocate empty vector and move data into it
    this->array_val = new std::vector<value>();
    *(this->array_val) = std::move(array_value);
}

value::value(const std::string& string_value) :
value_type(type::string) {
    std::string copy(string_value.data(), string_value.length());
    this->string_val = new std::string();
    *(this->string_val) = std::move(copy);
}

value::value(std::string&& string_value) :
value_type(type::string) {
    this->string_val = new std::string();
    *(this->string_val) = std::move(string_value);
}

value::value(const char* string_value) :
value_type(type::string) {
    this->string_val = new std::string(string_value);
}

value::value(int32_t integer_value) :
value(static_cast<int64_t> (integer_value)) { }

value::value(int64_t integer_value) :
value_type(type::integer), integer_val(integer_value) { }

value::value(uint32_t integer_value) :
value(static_cast<int64_t> (integer_value)) { }

value::value(int16_t integer_value) :
value(static_cast<int64_t> (integer_value)) { }

value::value(uint16_t integer_value) :
value(static_cast<int64_t> (integer_value)) { }

value::value(double real_value) :
value_type(type::real), real_val(real_value) { }

value::value(float real_value) :
value(static_cast<double> (real_value)) { }

value::value(bool boolean_value) :
value_type(type::boolean), boolean_val(boolean_value) { }

void value::dump(std::streambuf* dest) const {
    jansson_dump_to_streambuf(*this, dest);
}

std::string value::dumps() const {
    return jansson_dump_to_string(*this);
}

value value::clone() const {
    switch (value_type) {
    case type::nullt: return value();
    case type::object:
    {
        auto vec = std::vector<field>();
        vec.reserve(object_val->size());
        for (const field& fi : *object_val) {
            vec.emplace_back(fi.name(), fi.val().clone());
        }
        return value(std::move(vec));
    }
    case type::array:
    {
        auto vec = std::vector<value>();
        vec.reserve(array_val->size());
        for (const value& va : *array_val) {
            vec.emplace_back(va.clone());
        }
        return value(std::move(vec));
    }
    case type::string: return value(*string_val);
    case type::integer: return value(integer_val);
    case type::real: return value(real_val);
    case type::boolean: return value(boolean_val);
    default: return value();
    }
}

type value::json_type() const {
    return this->value_type;
}

const value& value::getattr(const std::string& name) const {
    for (auto& el : this->as_object()) {
        if (name == el.name()) {
            return el.val();
        }
    }
    return null_value;
}

const value& value::operator[](const std::string& name) const {
    return this->getattr(name);
}

value& value::getattr_or_throw(const std::string& name, const std::string& context) {
    if (type::object == value_type) {
        std::vector<field>& obj = this->as_object_or_throw();
        for (field& el : obj) {
            if (name == el.name()) {
                return el.val();
            }
        }
        // add attr
        obj.emplace_back(field(name, value()));
        return obj[obj.size() - 1].val();
    }
    // not object    
    throw json_exception(TRACEMSG("Cannot get attribute: [" + name + "]" +
            " from target value: [" + dumps() + "],"
            " context: [" + context + "]"));
}

const std::vector<field>& value::as_object() const {
    if (type::object == value_type) {
        return *(this->object_val);
    }
    return empty_object;
}

std::vector<field>& value::as_object_or_throw(const std::string& context) {
    return const_cast<std::vector<field>&> (const_cast<const value*> (this)->as_object_or_throw(context));
}

const std::vector<field>& value::as_object_or_throw(const std::string& context) const {
    if (type::object == value_type) {
        return *(this->object_val);
    }
    // not object    
    throw json_exception(TRACEMSG("Cannot access object" +
            " from target value: [" + dumps() + "],"
            " context: [" + context + "]"));
}

bool value::set_object(std::vector<field>&& object_value) {
    if (type::object == value_type) {
        *(this->object_val) = std::move(object_value);
        return true;
    }
    *this = value(std::move(object_value));
    return false;
}

const std::vector<value>& value::as_array() const {
    if (type::array == value_type) {
        return *(this->array_val);
    }
    return empty_array;
}

std::vector<value>& value::as_array_or_throw(const std::string& context) {
    return const_cast<std::vector<value>&> (const_cast<const value*> (this)->as_array_or_throw(context));
}

const std::vector<value>& value::as_array_or_throw(const std::string& context) const {
    if (type::array == value_type) {
        return *(this->array_val);
    }
    // not array    
    throw json_exception(TRACEMSG("Cannot access array" +
            " from target value: [" + dumps() + "]," +
            " context: [" + context + "]"));
}

bool value::set_array(std::vector<value>&& array_value) {
    if (type::array == value_type) {
        *(this->array_val) = std::move(array_value);
        return true;
    }
    *this = value(std::move(array_value));
    return false;
}

const std::string& value::as_string() const {
    if (type::string == value_type) {
        return *(this->string_val);
    }
    return empty_string;
}

std::string& value::as_string_or_throw(const std::string& context) {
    return const_cast<std::string&> (const_cast<const value*> (this)->as_string_or_throw(context));
}

const std::string& value::as_string_or_throw(const std::string& context) const {
    if (type::string == value_type) {
        return *(this->string_val);
    }
    // not string    
    throw json_exception(TRACEMSG("Cannot access string" +
            " from target value: [" + dumps() + "]," +
            " context: [" + context + "]"));
}

std::string& value::as_string_nonempty_or_throw(const std::string& context) {
    return const_cast<std::string&> (const_cast<const value*> (this)->as_string_nonempty_or_throw(context));
}

const std::string& value::as_string_nonempty_or_throw(const std::string& context) const {
    const std::string& res = as_string_or_throw(context);
    if (!res.empty()) {
        return res;
    }
    // empty
    throw json_exception(TRACEMSG("Specified string value is empty" +
            " context: [" + context + "]"));
}

const std::string& value::as_string(const std::string& default_val) const {
    if (type::string == value_type) {
        return *(this->string_val);
    }
    return default_val;
}

bool value::set_string(const std::string& string_value) {
    std::string copy(string_value.data(), string_value.length());
    return set_string(std::move(copy));
}

bool value::set_string(std::string&& string_value) {
    if (type::string == value_type) {
        *(this->string_val) = std::move(string_value);
        return true;
    }
    *this = value(std::move(string_value));
    return false;
}

int64_t value::as_int64() const {
    if (type::integer == value_type) {
        return this->integer_val;
    }
    return 0;
}

int64_t value::as_int64_or_throw(const std::string& context) const {
    if (type::integer == value_type) {
        return this->integer_val;
    }
    // not integer
    throw json_exception(TRACEMSG("Cannot access 'int64'" +
            " from target value: [" + dumps() + "]," +
            " context: [" + context + "]"));
}

int64_t value::as_int64(int64_t default_val) const {
    if (type::integer == value_type) {
        return this->integer_val;
    }
    return default_val;
}

bool value::set_int64(int64_t int_value) {
    if (type::integer == value_type) {
        this->integer_val = int_value;
        return true;
    }
    *this = value(int_value);
    return false;
}

int32_t value::as_int32() const {
    if (type::integer == value_type) {
        return static_cast<int32_t> (this->integer_val);
    }
    return 0;
}

int32_t value::as_int32_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sl::support::is_int32(val)) {
        return static_cast<int32_t> (val);
    }
    // not int32_t
    throw json_exception(TRACEMSG("Cannot access 'int32'" +
            " from target value: [" + dumps() + "]," +
            " context: [" + context + "]"));
}

int32_t value::as_int32(int32_t default_val) const {
    if (type::integer == value_type) {
        return static_cast<int32_t> (this->integer_val);
    }
    return default_val;
}

bool value::set_int32(int32_t int_value) {
    if (type::integer == value_type) {
        this->integer_val = int_value;
        return true;
    }
    *this = value(int_value);
    return false;
}

uint32_t value::as_uint32() const {
    if (type::integer == value_type) {
        return static_cast<uint32_t> (this->integer_val);
    }
    return 0;
}

uint32_t value::as_uint32_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sl::support::is_uint32(val)) {
        return static_cast<uint32_t> (val);
    }
    // not uint32_t
    throw json_exception(TRACEMSG("Cannot access 'uint32'" +
            " from target value: [" + dumps() + "]," +
            " context: [" + context + "]"));
}

uint32_t value::as_uint32_positive_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sl::support::is_uint32_positive(val)) {
        return static_cast<uint32_t> (val);
    }
    // not positive uint32_t
    throw json_exception(TRACEMSG("Cannot access positive 'uint32'" +
            " from target value: [" + dumps() + "]," +
            " context: [" + context + "]"));
}

uint32_t value::as_uint32(uint32_t default_val) const {
    if (type::integer == value_type) {
        return static_cast<uint32_t> (this->integer_val);
    }
    return default_val;
}

bool value::set_uint32(uint32_t uint_value) {
    if (type::integer == value_type) {
        this->integer_val = uint_value;
        return true;
    }
    *this = value(uint_value);
    return false;
}

int16_t value::as_int16() const {
    if (type::integer == value_type) {
        return static_cast<int16_t> (this->integer_val);
    }
    return 0;
}

int16_t value::as_int16_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sl::support::is_int16(val)) {
        return static_cast<int16_t> (val);
    }
    // not int16_t
    throw json_exception(TRACEMSG("Cannot access 'int16'" +
            " from target value: [" + dumps() + "]," +
            " context: [" + context + "]"));
}

int16_t value::as_int16(int16_t default_val) const {
    if (type::integer == value_type) {
        return static_cast<int16_t> (this->integer_val);
    }
    return default_val;
}

bool value::set_int16(int16_t int_value) {
    if (type::integer == value_type) {
        this->integer_val = int_value;
        return true;
    }
    *this = value(int_value);
    return false;
}

uint16_t value::as_uint16() const {
    if (type::integer == value_type) {
        return static_cast<uint16_t> (this->integer_val);
    }
    return 0;
}

uint16_t value::as_uint16_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sl::support::is_uint16(val)) {
        return static_cast<uint16_t> (val);
    }
    // not uint16_t
    throw json_exception(TRACEMSG("Cannot access 'uint16'" +
            " from target value: [" + dumps() + "]," +
            " context: [" + context + "]"));
}

uint16_t value::as_uint16_positive_or_throw(const std::string& context) const {
    int64_t val = this->as_int64_or_throw(context);
    if (sl::support::is_uint16_positive(val)) {
        return static_cast<int16_t> (val);
    }
    // not positive unt16_t
    throw json_exception(TRACEMSG("Cannot access 'int16'" +
            " from target value: [" + dumps() + "]," +
            " context: [" + context + "]"));
}

uint16_t value::as_uint16(uint16_t default_val) const {
    if (type::integer == value_type) {
        return static_cast<uint16_t> (this->integer_val);
    }
    return default_val;
}

bool value::set_uint16(uint16_t uint_value) {
    if (type::integer == value_type) {
        this->integer_val = uint_value;
        return true;
    }
    *this = value(uint_value);
    return false;
}

double value::as_double() const {
    if (type::real == value_type) {
        return this->real_val;
    }
    return 0;
}

double value::as_double_or_throw(const std::string& context) const {
    if (type::real == value_type) {
        return this->real_val;
    }
    // not real
    throw json_exception(TRACEMSG("Cannot access 'double'" +
            " from target value: [" + dumps() + "]," +
            " context: [" + context + "]"));
}

double value::as_double(double default_val) const {
    if (type::real == value_type) {
        return this->real_val;
    }
    return default_val;
}

bool value::set_double(double double_value) {
    if (type::real == value_type) {
        this->real_val = double_value;
        return true;
    }
    *this = value(double_value);
    return false;
}

float value::as_float() const {
    if (type::real == value_type) {
        return static_cast<float> (this->real_val);
    }
    return 0;
}

float value::as_float_or_throw(const std::string& context) const {
    double val = this->as_double_or_throw(context);
    if (val >= std::numeric_limits<float>::min() && val <= std::numeric_limits<float>::max()) {
        return static_cast<float> (val);
    }
    // not float
    throw json_exception(TRACEMSG("Cannot access 'float'" +
            " from target value: [" + dumps() + "]," +
            " context: [" + context + "]"));
}

float value::as_float(float default_val) const {
    if (type::real == value_type) {
        return static_cast<float> (this->real_val);
    }
    return default_val;
}

bool value::set_float(float float_value) {
    if (type::real == value_type) {
        this->real_val = float_value;
        return true;
    }
    *this = value(float_value);
    return false;
}

bool value::as_bool() const {
    if (type::boolean == value_type) {
        return this->boolean_val;
    }
    return false;
}

bool value::as_bool_or_throw(const std::string& context) const {
    if (type::boolean == value_type) {
        return this->boolean_val;
    }
    // not boolean
    throw json_exception(TRACEMSG("Cannot access 'boolean'" +
            " from target value: [" + dumps() + "]," +
            " context: [" + context + "]"));
}

bool value::as_bool(bool default_val) const {
    if (type::boolean == value_type) {
        return this->boolean_val;
    }
    return default_val;
}

bool value::set_bool(bool bool_value) {
    if (type::boolean == value_type) {
        this->boolean_val = bool_value;
        return true;
    }
    *this = value(bool_value);
    return false;
}

}
} // namespace
