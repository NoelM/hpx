// cache_before_init.cpp

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

#include <hpx/config.hpp>

#include <hpx/util/logging/detail/cache_before_init.hpp>
#include <hpx/util/logging/format/optimize.hpp>
#include <hpx/util/logging/writer/named_write.hpp>

#include <utility>

#if defined (HPX_WINDOWS)
#include <windows.h>
#endif

namespace hpx { namespace util { namespace logging { namespace detail {

inline thread_id_type get_thread_id()
{
#if defined (HPX_WINDOWS)
    return ::GetCurrentThreadId();
#else
    return pthread_self ();
#endif
}

bool cache_before_init::is_cache_turned_off() const {
    if ( m_is_caching_off )
        return true; // cache has been turned off

    // now we go the slow way - see if cache is turned off
    m_is_caching_off = !(m_cache.is_using_cache);
    return m_is_caching_off;
}

void cache_before_init::turn_cache_off(const writer::named_write & writer_) {
    if ( is_cache_turned_off() )
        return; // already turned off

    {
        m_cache.is_using_cache = false;
    }

    // dump messages
    typename cache::message_array msgs;
    {
        std::swap( m_cache.msgs, msgs);
    }
    for ( typename cache::message_array::iterator b = msgs.begin(),
        e = msgs.end(); b != e; ++b) {
        if ( !(b->is_enabled) )
            // no filter
            writer_( b->string );
        else if ( b->is_enabled() )
            // filter enabled
            writer_( b->string );
    }
}

void cache_before_init::add_msg(const msg_type & msg) const {
    // note : last_enabled can be null, if we don't want to use filters
    is_enabled_func func = m_cache.threads[ get_thread_id() ].last_enabled ;
    m_cache.msgs.push_back( message(func, msg) );
}

void cache_before_init::set_callback(is_enabled_func f) {
    m_cache.threads[ get_thread_id() ].last_enabled = f;
}

}}}}
