Staticlibs serialization library
================================

This project is a part of [Staticlibs](http://staticlibs.net/).

This project contains high level `dump_*` and `load_*` functions to convert 
objects to/from raw data. Currently only JSON serialization (based on [Jansson library](https://github.com/akheron/jansson))
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

[pkg-config](http://www.freedesktop.org/wiki/Software/pkg-config/) utility is used for dependency management.
For Windows users ready-to-use binary version of `pkg-config` can be obtained from [tools_windows_pkgconfig](https://github.com/staticlibs/tools_windows_pkgconfig) repository.
See [PkgConfig](https://github.com/staticlibs/wiki/wiki/PkgConfig) for Staticlibs-specific details about `pkg-config` usage.

This project depends on a [jansson](https://github.com/akheron/jansson) and on a number of Staticlib libraries.

See [StaticlibsDependencies](https://github.com/staticlibs/wiki/wiki/StaticlibsDependencies) for more 
details about dependency management with Staticlibs.

To build this project manually:

 * checkout all the dependent projects
 * configure these projects using the same output directory:

Run:

    mkdir build
    cd build
    cmake .. -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=<my_lib_dir>

 * build all the dependent projects
 * configure this projects using the same output directory and build it:

To build the library on Windows using Visual Studio 2013 Express run the following commands using
Visual Studio development command prompt 
(`C:\Program Files (x86)\Microsoft Visual Studio 12.0\Common7\Tools\Shortcuts\VS2013 x86 Native Tools Command Prompt`):

    git clone https://github.com/staticlibs/staticlib_serialization.git
    cd staticlib_serialization
    mkdir build
    cd build
    cmake .. -DCMAKE_LIBRARY_OUTPUT_DIRECTORY=<my_lib_dir>
    msbuild staticlib_serialization.sln

To build on other platforms using GCC or Clang with GNU Make:

    cmake .. -DCMAKE_CXX_FLAGS="--std=c++11"
    make

See [StaticlibsToolchains](https://github.com/staticlibs/wiki/wiki/StaticlibsToolchains) for 
more information about the toolchain setup and cross-compilation.

License information
-------------------

This project is released under the [Apache License 2.0](http://www.apache.org/licenses/LICENSE-2.0)

Changelog
---------

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
