
cmake_minimum_required ( VERSION 2.8.12 )

# toolchain setup
set ( STATICLIB_TOOLCHAIN linux_amd64_clang CACHE STRING "toolchain triplet" )
set ( CMAKE_TOOLCHAIN_FILE ${CMAKE_CURRENT_LIST_DIR}/cmake/toolchains/${STATICLIB_TOOLCHAIN}.cmake CACHE INTERNAL "" )

# project setup
project ( staticlib_serialization CXX )
# common include
include ( ${CMAKE_CURRENT_LIST_DIR}/cmake/CMakeLists_common.cmake )

# dependencies
staticlib_add_subdirectory ( external_jansson )
staticlib_add_subdirectory ( staticlib_utils )
staticlib_add_subdirectory ( staticlib_reflection )

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
