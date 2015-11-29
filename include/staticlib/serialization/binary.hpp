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
 * File:   binary.hpp
 * Author: alex
 *
 * Created on November 28, 2015, 7:36 PM
 */

#ifndef STATICLIB_SERIALIZATION_BINARY_HPP
#define	STATICLIB_SERIALIZATION_BINARY_HPP

#include <memory>
#include <type_traits>

#include "staticlib/config.hpp"
#include "staticlib/io.hpp"
#include "staticlib/ranges.hpp"

#include "staticlib/serialization/SerializationException.hpp"

namespace staticlib {
namespace serialization {

namespace detail_binary {

template <typename Source, typename Pod>
class pod_range : public staticlib::ranges::range_adapter<pod_range<Source, Pod>, Pod> {
    Source source;
    typename std::aligned_storage<sizeof (Pod), std::alignment_of<Pod>::value>::type pod_space;

public:
    pod_range(Source source) :
    source(std::move(source)) { }

    bool compute_next() {
        std::streamsize read = staticlib::io::read_all(source, reinterpret_cast<char*>(std::addressof(pod_space)), sizeof(Pod));
        switch (read) {
        case sizeof(Pod):
            return this->set_current(std::move(*reinterpret_cast<Pod*>(std::addressof(pod_space))));
        case 0:
            return false;
        default:
            throw SerializationException(TRACEMSG(std::string() +
                    "Invalid read of size: [" + staticlib::config::to_string(read) + "],"
                    " expected size: [" + staticlib::config::to_string(sizeof(Pod)) + "]"));
        }
    }
};

} // namespace

template <typename Range, typename Sink>
void dump_binary(Range& range, Sink& sink) {
    static_assert(std::is_pod<typename Range::value_type>::value, "Range element type must be a POD type");
    for(auto&& el : range) {
        staticlib::io::write_all(sink, reinterpret_cast<char*>(std::addressof(el)), sizeof(typename Range::value_type));
    }
}

template <typename Pod, typename Source>
detail_binary::pod_range<Source, Pod> load_binary(Source&& src) {
    static_assert(std::is_pod<Pod>::value, "Range element type must be a POD type");
    return detail_binary::pod_range<Source, Pod>(std::move(src));
}

} // namespace
}

#endif	/* STATICLIB_SERIALIZATION_BINARY_HPP */

