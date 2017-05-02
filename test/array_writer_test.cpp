/* 
 * File:   array_writer_test.cpp
 * Author: alex
 *
 * Created on May 2, 2017, 9:19 PM
 */

#include "staticlib/json/array_writer.hpp"

#include <iostream>

#include "staticlib/config/assert.hpp"
#include "staticlib/io.hpp"

#include "staticlib/json/field.hpp"

void test_empty() {
    auto sink = sl::io::string_sink();
    {
        auto writer = sl::json::make_array_writer(sink);
        (void) writer;
    }    
    slassert("[\n\n]\n" == sink.get_string());
}

void test_one() {
    auto sink = sl::io::string_sink();
    {
        auto writer = sl::json::make_array_writer(sink);
        writer.write({
            {"foo", 42}
        });
    }
    slassert(std::string::npos == sink.get_string().find(','));
}

void test_multiple() {
    auto sink = sl::io::string_sink();
    {
        auto writer = sl::json::make_array_writer(sink);
        writer.write({
            {"foo", 42}
        });
        writer.write({
            {"bar", "baz"}
        });
    }
    slassert(std::string::npos != sink.get_string().find(','));
}

int main() {
    try {
        test_empty();
        test_one();
        test_multiple();
    } catch (const std::exception& e) {
        std::cout << e.what() << std::endl;
        return 1;
    }
    return 0;
}
