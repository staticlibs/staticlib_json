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
 * File:   jansson_hashtable_helper.h
 * Author: alex
 *
 * Created on September 12, 2015, 7:02 PM
 */

#ifndef STATICLIB_JANSSON_HASHTABLE_HELPER_H
#define	STATICLIB_JANSSON_HASHTABLE_HELPER_H

// some logic from "hashtable.h", used for dict keys ordering, not exposed from .so
extern "C" {

struct hashtable_list {
    struct hashtable_list *prev;
    struct hashtable_list *next;
};
typedef struct hashtable_list list_t;

/* "pair" may be a bit confusing a name, but think of it as a
   key-value pair. In this case, it just encodes some extra data,
   too */
extern "C" struct hashtable_pair {
    size_t hash;
    struct hashtable_list list;
    json_t *value;
    size_t serial;
    char key[1];
};
typedef struct hashtable_pair pair_t;

#define container_of(ptr_, type_, member_)  \
        ((type_ *)((char *)ptr_ - offsetof(type_, member_)))

#define list_to_pair(list_)  container_of(list_, pair_t, list)

static size_t hashtable_iter_serial(void *iter) {
    pair_t *pair = list_to_pair((list_t *) iter);
    return pair->serial;
}

}

#endif	/* STATICLIB_JANSSON_HASHTABLE_HELPER_H */

