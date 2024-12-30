#pragma once

// Windows building a shared lib (.dll)
#if defined(USBTINGO_SHARED) && (defined(_WIN32) || defined(_WIN64))
    // Disable compiler warning C4251 caused by members of standard library
    // https://learn.microsoft.com/cpp/error-messages/compiler-warnings/compiler-warning-level-1-c4251
    // #pragma warning(disable : 4251)

    #ifdef USBTINGO_EXPORT
        #define USBTINGO_API __declspec(dllexport)
    #else
        #define USBTINGO_API __declspec(dllimport)
    #endif

// Linux building a shared lib (.so)
// Linux building a static lib (.a)
// Windows building a static lib (.lib)
#else
    #define USBTINGO_API
#endif