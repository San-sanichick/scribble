#pragma once


#ifdef DEBUG_BUILD
    #include "pch.hpp"

    #define CORE_ABORT() abort()



    #define CORE_LOG(x) std::cout << __FILE__ << ':' << __LINE__ << "> " << x
    #define CORE_LOGN(x) std::cout << __FILE__ << ':' << __LINE__ << "> " << x << std::endl
    #define CORE_ERR(x) std::cerr << __FILE__ << ':' << __LINE__ << "> " <<  x
    #define CORE_ERRN(x) std::cerr << __FILE__ << ':' << __LINE__ << "> " <<  x << std::endl

    #define CORE_ASSERT_LOG(cond, msg) \
        std::cout << "Assertion '" << cond << "' failed in" << __FILE__ << ':' << __LINE__ << "> " << msg << std::endl


    #define CORE_ASSERT(x, message) do { if (!(x)) { CORE_ASSERT_LOG(#x, message); CORE_ABORT(); } } while(0)
#else
    #define CORE_LOG(x)
    #define CORE_LOGN(x)
    #define CORE_ERR(x)
    #define CORE_ERRN(x)

    #define CORE_ASSERT(x, msg)
#endif
