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
 * File:   array_writer.hpp
 * Author: alex
 *
 * Created on May 2, 2017, 8:43 PM
 */

#ifndef STATICLIB_JSON_ARRAY_WRITER_HPP
#define	STATICLIB_JSON_ARRAY_WRITER_HPP

#include "staticlib/config.hpp"
#include "staticlib/io.hpp"

#include "staticlib/json/value.hpp"

namespace staticlib {
namespace json {

/**
 * Writes entries to the underlying sink formatting the output as
 * JSON array. No more than a single entry is kept in memory in at the same time.
 * Entries are dumped directly to underlying sink without intermediate string representation.
 */
template<typename Sink>
class array_writer {
    Sink sink;
    bool first_entry_written = false;
    
public:
    array_writer(Sink&& sink) :
    sink(std::move(sink)) {
        sl::io::write_all(sink, {"[\n"});
    }
    
    ~array_writer() STATICLIB_NOEXCEPT {
        try {
            sl::io::write_all(sink, {"\n]\n"});
        } catch (...) {
            // keep silent
        }
    }

    /**
     * Deleted copy constructor
     * 
     * @param other instance
     */
    array_writer(const array_writer&) = delete;

    /**
     * Deleted copy assignment operator
     * 
     * @param other instance
     * @return this instance 
     */
    array_writer& operator=(const array_writer&) = delete;
    
    /**
     * Move constructor
     * 
     * @param other other instance
     */
    array_writer(array_writer&& other) :
    sink(std::move(other.sink)),
    first_entry_written(other.first_entry_written) { }
    
    /**
     * Move assignment operator
     * 
     * @param other other instance
     * @returns this instance
     */
    array_writer& operator=(array_writer&& other) {
        sink = std::move(other.sink);
        first_entry_written = other.first_entry_written;
        return *this;
    }
    
    /**
     * Writes specified json value to underlying sink
     * 
     * @param entry json value to write to sink
     */
    void write(const value& entry) {
        if (first_entry_written) {
            sl::io::write_all(sink, {",\n"});
        } else {
            first_entry_written = true;
        }
        entry.dump(sink);
    }
    
};

/**
 * Factory function for creating array writers,
 * created writer will own specified sink
 * 
 * @param sink destination sink
 * @return writer instance
 */
template <typename Sink,
class = typename std::enable_if<!std::is_lvalue_reference<Sink>::value>::type>
array_writer<Sink> make_array_writer(Sink&& sink) {
    return array_writer<Sink>(std::move(sink));
}

/**
 * Factory function for creating array writers,
 * created writer will NOT own specified sink
 * 
 * @param sink destination sink
 * @return writer instance
 */
template <typename Sink>
array_writer<sl::io::reference_sink<Sink>> make_array_writer(Sink& sink) {
    return array_writer<sl::io::reference_sink<Sink>> (sl::io::make_reference_sink(sink));
}

} // namespace
}

#endif	/* STATICLIB_JSON_ARRAY_WRITER_HPP */

