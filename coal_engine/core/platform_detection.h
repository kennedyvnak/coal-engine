#if defined(_WIN32) || defined(_WIN64)
    #define ENGINE_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define ENGINE_PLATFORM_LINUX
#elif defined(__APPLE__) || defined(__MACH__)
    #include <TargetConditionals.h>
    #if TARGET_IPHONE_SIMULATOR == 1
        #error "iOS simulator is not supported!"
    #elif TARGET_OS_IPHONE == 1
        #define ENGINE_PLATFORM_IOS
        #error "iOS is not supported!"
    #elif TARGET_OS_MAC == 1
        #define ENGINE_PLATFORM_MACOS
    #else
        #error "Unknown Apple platform!"
    #endif
#else
    #error "Non supported platform."
#endif

#ifdef ENGINE_PLATFORM_WINDOWS
    #define EN_DEBUG_BREAK() __debugbreak()
#elif defined(ENGINE_PLATFORM_LINUX)
    #include <signal.h>
    #define EN_DEBUG_BREAK() raise(SIGTRAP)
#elif defined(ENGINE_PLATFORM_MACOS)
    #define EN_DEBUG_BREAK() __builtin_trap()
#endif