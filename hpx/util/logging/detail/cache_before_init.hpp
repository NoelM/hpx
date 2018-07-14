// cache_before_init.hpp

// Boost Logging library
//
// Author: John Torjo, www.torjo.com
//
// Copyright (C) 2007 John Torjo (see www.torjo.com for email)
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org for updates, documentation, and revision history.
// See http://www.torjo.com/log2/ for more details

#ifndef JT28092007_cache_before_init_HPP_DEFINED
#define JT28092007_cache_before_init_HPP_DEFINED

#include <hpx/config.hpp>
#include <hpx/util/logging/detail/fwd.hpp>
#include <hpx/util/logging/format_fwd.hpp>
#include <hpx/util/logging/format/optimize.hpp>

#include <map>
#include <utility>
#include <vector>

#if defined (HPX_WINDOWS)
#include <windows.h>
#endif

#include <hpx/config/warnings_prefix.hpp>

namespace hpx { namespace util { namespace logging { namespace detail {

#if defined (HPX_WINDOWS)
typedef DWORD thread_id_type;
#else
typedef pthread_t thread_id_type;
#endif

typedef bool (*is_enabled_func)();

struct thread_info {
    thread_info() : last_enabled(nullptr) {}
    is_enabled_func last_enabled;
};

//////////////////////////////////////////////////////////////////
// Messages that were logged before initializing the log - Caching them

/**
    The library will make sure your logger derives from this in case you want to
    cache messages that are logged before logs are initialized.

    Note:
    - you should initialize your logs ASAP
    - before logs are initialized
    - cache can be turned off ONLY ONCE
*/
struct HPX_EXPORT cache_before_init {
private:
    struct message {
        message(is_enabled_func is_enabled_, msg_type string_)
            : is_enabled(is_enabled_), string(string_) {}
        // function that sees if the filter is enabled or not
        is_enabled_func is_enabled;
        // the message itself
        msg_type string;
    };

    struct cache {
        cache() : is_using_cache(true) {}

        typedef std::map<thread_id_type, thread_info> thread_coll;
        thread_coll threads;

        typedef std::vector<message> message_array;
        message_array msgs;

        bool is_using_cache;
    };

public:
    cache_before_init() : m_is_caching_off(false) {}

    bool is_cache_turned_off() const;

    void turn_cache_off(const writer::named_write & writer_);

    void add_msg(const msg_type & msg) const;

    void set_callback(is_enabled_func f);

private:
    mutable cache m_cache;
    /**
    IMPORTANT: to make sure we know when the cache is off as efficiently as possible,
    I have this mechanism:
    - first, query m_is_enabled, which at the beginning is false
      - if this is true, it's clear that caching has been turned off
      - if this is false, we don't know for sure, thus, continue to ask

    - second, use the thread-safe resource 'm_cache'
      - if m_cache.is_using_cache is true, we're still using cache
      - if m_cache.is_using_cache is false, caching has been turned off
        - set m_is_enabled to true, thus this will propagate to all threads soon
    */
    mutable bool m_is_caching_off;
};

}}}}

#include <hpx/config/warnings_suffix.hpp>

#endif
