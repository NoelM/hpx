// log_keeper.hpp

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


#ifndef JT28092007_log_keeper_HPP_DEFINED
#define JT28092007_log_keeper_HPP_DEFINED

#include <hpx/util/logging/detail/fwd.hpp>
#include <hpx/util/logging/detail/forward_constructor.hpp>

#include <cstdint>

namespace hpx { namespace util { namespace logging {


/**
    @brief Allows using a log without knowing its full type yet.
    Even if the log is not fully @b defined, you can still use it.

    This will allow you to log messages even if you don't know the full type of the log
    (which can aid compilation time).

    This is a base class. Use logger_holder_by_value or logger_holder_by_ptr instead
*/
template<class type> struct logger_holder {

    const type* operator->() const      { return m_log; }
    type* operator->()                  { return m_log; }

    /**
        in case you want to get the real log object
    */
    const type* get() const             { return m_log; }
    type* get()                         { return m_log; }

protected:
    logger_holder() : m_log(nullptr) {}
    virtual ~logger_holder() {}

    void init(type * log) {
        m_log = log;
    }
private:
    // note: I have a pointer to the log, as opposed to having it by value,
    // because the whole purpose of this class
    // is to be able to use a log without knowing its full type
    type *m_log;
};



/**
    @brief Allows using a log without knowing its full type yet.
    Even if the log is not fully @b defined, you can still use it.

    This will allow you to log messages even if you don't know the full type of the log
    (which can aid compilation time).
*/
template<class type> struct logger_holder_by_value : logger_holder<type> {
    typedef logger_holder<type> base_type;

    HPX_LOGGING_FORWARD_CONSTRUCTOR_INIT(logger_holder_by_value, m_log_value, init)
private:
    void init() {
        base_type::init( &m_log_value);
    }
private:
    // VERY IMPORTANT: we keep this BY VALUE, because,
    // at destruction, we don't want the memory to be freed
    // (in order for the after_being_destroyed to work,
    //  for global instances of this type)
    type m_log_value;
};


/**
    @brief Ensures the log is created before main(), even if not used before main

    We need this, so that we won't run into multi-threaded issues while
    the log is created
    (in other words, if the log is created before main(),
    we can safely assume there's only one thread running,
    thus no multi-threaded issues)
*/
struct ensure_early_log_creation {
    template<class type> ensure_early_log_creation ( type & log) {
    typedef std::int64_t long_type ;
        long_type ignore = reinterpret_cast<long_type>(&log);
        // we need to force the compiler to force creation of the log
        if ( time(nullptr) < 0)
            if ( time(nullptr) < (time_t)ignore) {
                printf("LOGGING LIB internal error - should NEVER happen. \
                    Please report this to the author of the lib");
                exit(0);
            }
    }
};


/**
    @brief Ensures the filter is created before main(), even if not used before main

    We need this, so that we won't run into multi-threaded issues while
    the filter is created
    (in other words, if the filter is created before main(),
    we can safely assume there's only one thread running,
    thus no multi-threaded issues)
*/
typedef ensure_early_log_creation ensure_early_filter_creation;


}}}

#endif
