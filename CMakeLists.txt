# Copyright 2015, alex at staticlibs.net
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

cmake_minimum_required ( VERSION 2.8.12 )

# toolchain setup
set ( STATICLIB_TOOLCHAIN linux_amd64_gcc CACHE STRING "toolchain triplet" )
set ( CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/toolchains/${STATICLIB_TOOLCHAIN}.cmake CACHE INTERNAL "" )

# project setup
project ( staticlib_serialization CXX )
set ( ${PROJECT_NAME}_STATICLIB_VERSION 1.0.2 )
# common include
include ( ${CMAKE_CURRENT_LIST_DIR}/cmake/staticlibs_common.cmake )

# options, use SET ( OPTNAME ON CACHE BOOL "") in parent to override
option ( STATICLIB_WITH_ICU "Build with ICU support" OFF )

# dependencies
if ( STATICLIB_WITH_ICU )
    staticlib_add_subdirectory ( deps external_icu )
endif ( )
staticlib_add_subdirectory ( deps external_jansson )
staticlib_add_subdirectory ( deps staticlib_utils )
staticlib_add_subdirectory ( deps staticlib_reflection )

# build steps
file ( GLOB_RECURSE ${PROJECT_NAME}_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp )
add_library ( ${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRC} )
set ( ${PROJECT_NAME}_OPTIONS "" )
set ( ${PROJECT_NAME}_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/include 
        ${staticlib_utils_INCLUDES} 
        ${staticlib_reflection_INCLUDES} )
if ( STATICLIB_TOOLCHAIN MATCHES "windows_[^_]+_msvc" )
    set ( ${PROJECT_NAME}_OPTIONS ${${PROJECT_NAME}_OPTIONS} /wd4706 )
endif ( )
if ( STATICLIB_WITH_ICU )
    set ( ${PROJECT_NAME}_OPTIONS ${${PROJECT_NAME}_OPTIONS} ${external_icu_OPTIONS} -DSTATICLIB_WITH_ICU )
    set ( ${PROJECT_NAME}_INCLUDES ${${PROJECT_NAME}_INCLUDES} ${external_icu_INCLUDES} )
endif ( )
target_compile_options ( ${PROJECT_NAME} PRIVATE ${${PROJECT_NAME}_OPTIONS} )
target_include_directories ( ${PROJECT_NAME} BEFORE PRIVATE 
        ${${PROJECT_NAME}_INCLUDES}
        ${CMAKE_CURRENT_LIST_DIR}/src
        ${external_jansson_INCLUDES} )

# tests and docs
set ( ${PROJECT_NAME}_TEST_INCLUDES ${${PROJECT_NAME}_INCLUDES} 
        ${CMAKE_CURRENT_LIST_DIR}/src 
        ${external_jansson_INCLUDES} )
set ( ${PROJECT_NAME}_TEST_LIBS ${PROJECT_NAME}
        staticlib_reflection
        staticlib_utils
        jansson )
set ( ${PROJECT_NAME}_TEST_OPTS "" )
if ( STATICLIB_WITH_ICU )
    set ( ${PROJECT_NAME}_TEST_LIBS ${${PROJECT_NAME}_TEST_LIBS} external_icu external_icu_data )
    set ( ${PROJECT_NAME}_TEST_OPTS ${${PROJECT_NAME}_TEST_OPTS} ${external_icu_OPTIONS} -DSTATICLIB_WITH_ICU )
endif ( )
staticlib_enable_testing ( ${PROJECT_NAME}_TEST_INCLUDES ${PROJECT_NAME}_TEST_LIBS ${PROJECT_NAME}_TEST_OPTS )
staticlib_enable_docs ( )

# export headers
set ( ${PROJECT_NAME}_INCLUDES ${${PROJECT_NAME}_INCLUDES} CACHE INTERNAL "" )
