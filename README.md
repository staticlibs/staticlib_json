Staticlibs JSON library
=======================

[![travis](https://travis-ci.org/staticlibs/staticlib_json.svg?branch=master)](https://travis-ci.org/staticlibs/staticlib_json)
[![appveyor](https://ci.appveyor.com/api/projects/status/github/staticlibs/staticlib_json?svg=true)](https://ci.appveyor.com/project/staticlibs/staticlib-json)

This project is a part of [Staticlibs](http://staticlibs.net/).

This project implements JSON encoding and decoding to/from `std::string`s or arbitrary `Source`s and `Sink`s
(see [staticlib_io](https://github.com/staticlibs/staticlib_io) for details about streaming).

Tagged union is used for in-memory JSON representation. [Jansson library](https://github.com/akheron/jansson) is used
for JSON processing.

Link to the [API documentation](http://staticlibs.github.io/staticlib_json/docs/html/namespacestaticlib_1_1json.html).

JSON encoding
-------------

Method to covert object to in-memory `sl::json::value`:

    sl::json::value to_json() const {
        return {
            {"field1", field1},
            {"field2", field3},
            {"additional_field", 42}
            ...
        };
    }

Use the method above to encode object into JSON string:

    sl::json::value jval = myobj.to_json();
    std::string str = jval.dumps();

JSON decoding
-------------

Implement constructor that takes `sl::json::value`:

_Note: for most of the modern compilers this loop can be improved using `switch` and `constexpr`
expressions, but this won't work for MS Visual Studio 2013._

    my_class(const sl::json::value& val) {
        for(auto& fi : val.as_object()) {
            auto name = fi.name();
            if("f1" == name) {
                this->f1 = fi.as_uint32_or_throw();
            } else if("f2" == name) {
                this->f2 = fi.as_string_or_throw();
            }
            ...
        }
    }

Parse JSON string and instantiate object:

    sl::json::value jval = sl::json::loads(str);
    my_class myobj{jval};

Fluent API
----------

Example of using "fluent" API for inspecting object graph:

    const sl::json::value& obj = sl::json::loads(...);
    std::string& bazval = obj["foo"]["bar"]["baz"].as_string();

Note: `operator[]` has `O(n)` complexity where `n` is a number of attributes inside the object.

How to build
------------

[CMake](http://cmake.org/) is required for building.

[pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/) utility is used for dependency management.
For Windows users ready-to-use binary version of `pkg-config` can be obtained from [tools_windows_pkgconfig](https://github.com/staticlibs/tools_windows_pkgconfig) repository.
See [StaticlibsPkgConfig](https://github.com/staticlibs/wiki/wiki/StaticlibsPkgConfig) for Staticlibs-specific details about `pkg-config` usage.

To build the library on Windows using Visual Studio 2013 Express run the following commands using
Visual Studio development command prompt 
(`C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\Shortcuts\VS2013 x86 Native Tools Command Prompt`):

    git clone --recursive https://github.com/staticlibs/external_jansson.git
    git clone https://github.com/staticlibs/staticlib_config.git
    git clone https://github.com/staticlibs/staticlib_support.git
    git clone https://github.com/staticlibs/staticlib_io.git
    git clone https://github.com/staticlibs/staticlib_json.git
    cd staticlib_json
    mkdir build
    cd build
    cmake .. 
    msbuild staticlib_json.sln

Cloning of [external_jansson](https://github.com/staticlibs/external_jansson.git) is not required on Linux - 
system Jansson library will be used instead.

To build on other platforms using GCC or Clang with GNU Make:

    cmake .. -DCMAKE_CXX_FLAGS="--std=c++11"
    make

See [StaticlibsToolchains](https://github.com/staticlibs/wiki/wiki/StaticlibsToolchains) for 
more information about the CMake toolchains setup and cross-compilation.

License information
-------------------

This project is released under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0).

Changelog
---------

**2018-01-26**

 * version 2.1.2
 * jansson 2.8+ compat, init dropped

**2017-12-24**

 * version 2.1.1
 * vs2017 support

**2017-05-02**

 * version 2.1.0
 * `array_writer` added

**2017-04-08**

 * version 2.0.0
 * renamed to `staticlib_json`
 * simplify encoding/decoding api
 * ICU support dropped
 * binary encoding dropped

**2016-09-01**

 * version 1.5.1
 * make API more fluent
 * extend setters API

**2016-06-29**

 * version 1.5.0
 * ICU support reworked

**2016-01-22**

 * version 1.4.3
 * minor CMake changes

**2015-12-13**

 * version 1.4.2
 * fix for templated `Sink`/`Source` operations

**2015-12-04**

 * version 1.4.1
 * shorter naming for shortcut `get` operation

**2015-11-29**

 * version 1.4.0
 * binary format support
 * arbitrary `Sink`s/`Source`s support for a JSON format
 * `JsonValue` and `JsonField` added to source tree

**2015-11-19**

 * version 1.3.2
 * streaming support for `icu::UnicodeString`

**2015-11-17**

 * version 1.3.1
 * deplibs cache support

**2015-11-09**

 * version 1.3.0
 * use streaming for Jansson 2.4+

**2015-11-06**

 * version 1.2.1
 * fix build with bundled jansson

**2015-11-01**

 * version 1.2.0
 * `pkg-config` integration

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
