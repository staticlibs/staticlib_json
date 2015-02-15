
cmake_minimum_required ( VERSION 2.8.12 )

# toolchain setup
set ( STATICLIB_TOOLCHAIN linux_amd64_clang CACHE STRING "toolchain triplet" )
set ( CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/toolchains/${STATICLIB_TOOLCHAIN}.cmake CACHE INTERNAL "" )

# project setup
project ( staticlib_serialization CXX )
# common include
include ( ${CMAKE_CURRENT_LIST_DIR}/cmake/CMakeLists_common.cmake )

# dependencies
if ( NOT IS_DIRECTORY ${CMAKE_BINARY_DIR}/external_jansson )
    add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/deps/external_jansson ${CMAKE_BINARY_DIR}/external_jansson )
endif ( )
if ( NOT IS_DIRECTORY ${CMAKE_BINARY_DIR}/staticlib_utils )
    add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/deps/staticlib_utils ${CMAKE_BINARY_DIR}/staticlib_utils )
endif ( )
if ( NOT IS_DIRECTORY ${CMAKE_BINARY_DIR}/staticlib_reflection )
    add_subdirectory ( ${CMAKE_CURRENT_LIST_DIR}/deps/staticlib_reflection ${CMAKE_BINARY_DIR}/staticlib_reflection )
endif ( )

# build steps
file ( GLOB_RECURSE ${PROJECT_NAME}_SRC ${CMAKE_CURRENT_LIST_DIR}/src/*.cpp )
add_library ( ${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SRC} )
set ( ${PROJECT_NAME}_INCLUDES ${CMAKE_CURRENT_LIST_DIR}/include 
        ${staticlib_utils_INCLUDES} 
        ${staticlib_reflection_INCLUDES} )
target_include_directories ( ${PROJECT_NAME} BEFORE PRIVATE ${${PROJECT_NAME}_INCLUDES}
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
staticlib_enable_testing ( ${PROJECT_NAME}_TEST_INCLUDES ${PROJECT_NAME}_TEST_LIBS )
staticlib_enable_docs ( )

# export headers
set ( ${PROJECT_NAME}_INCLUDES ${${PROJECT_NAME}_INCLUDES} CACHE INTERNAL "" )
