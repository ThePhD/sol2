#ifndef EXAMPLES_ASSERT_HPP
#define EXAMPLES_ASSERT_HPP

#ifndef NDEBUG
#include <exception>
#include <iostream>

#   define m_assert(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::terminate(); \
        } \
    } while (false)

#   define c_assert(condition) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << std::endl; \
            std::terminate(); \
        } \
    } while (false)
#else
#   define m_assert(condition, message) do { (void)sizeof(condition); (void)sizeof(message); } while (false)
#   define c_assert(condition) do { (void)sizeof(condition); } while (false)
#endif

#endif // EXAMPLES_ASSERT_HPP
