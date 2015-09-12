Staticlibs serialization library
================================

This project is a part of [Staticlibs](http://staticlibs.net/).

This project contains high level `dumps_*` and `loads_*` functions to convert 
objects to/from strings. Currently only JSON serialization (based on [Jansson library](https://github.com/akheron/jansson))
is supported. More formats may be added in future.

Link to [API documentation](http://staticlibs.github.io/staticlib_serialization/docs/html/namespacestaticlib_1_1serialization.html).

Serialization implementation
-------------------------

Serialization is implemented through [Staticlibs reflection library](https://github.com/staticlibs/staticlib_serialization).
To support serialization objects should implement converting to/from "reflected representation".

Example of implementing `to_reflected` logic using C++11 literals:

    sr::ReflectedValue get_reflected_value() const {
        return {
            {"field1", field1},
            {"field2", field3},
            {"additional_field", 42}
            ...
        };
    }

Example implementing `from_reflected` constructor using a loop over `name`->`value` pairs:

    MyClass(const ReflectedValue& val) {
        for(const auto& fi : val.get_object()) {
            auto name = fi.get_name();                
            if("f1" == name) {
                this->f1 = fi.get_value().get_integer();
            } else if("f2" == name) {
                this->f2 = fi.get_value().get_string();
            }
            ...
        }
    }

For most modern compilers this loop can be optimized using `switch` and `constexpr`
expressions, but this won't work for MS Visual Studio 2013.

How to build
------------

[CMake](http://cmake.org/) is required for building.

[TODO]

License information
-------------------

This project is released under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0)

Changelog
---------

**2015-09-05**

 * version 1.1.1 - removed dependency on utils

**2015-09-03**

 * version 1.1.0 - ICU support

**2015-07-09**

 * version 1.0.2 - toolchains update

**2015-06-30**

 * version 1.0.1 - toolchains update

**2015-05-19**

 * 1.0.0, initial public version
