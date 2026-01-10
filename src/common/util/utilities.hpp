#pragma once

#include <memory>
#include <chrono>

//DISABLE_COPY macro
#ifndef DISABLE_COPY
#define DISABLE_COPY(T)										\
			explicit T(const T&) = delete;					\
			T& operator=(const T&) = delete;
#endif

//DISABLE_MOVE macro
#ifndef DISABLE_MOVE
#define DISABLE_MOVE(T)										\
			explicit T(T&&) = delete;						\
			T& operator=(T&&) = delete;
#endif

//DISABLE_COPY_AND_MOVE macro
#ifndef DISABLE_COPY_AND_MOVE
#define DISABLE_COPY_AND_MOVE(T) DISABLE_COPY(T) DISABLE_MOVE(T)
#endif

#ifndef CLOCK_NS
#define CLOCK_NS std::chrono::high_resolution_clock::now().time_since_epoch().count() 
#endif

#ifndef CLOCK_MS
#define CLOCK_MS (CLOCK_NS / 1000000)
#endif

#ifndef FIXED_TIMESTEP_MS
#define FIXED_TIMESTEP_MS 16
#endif

#ifndef DELETER_EMPTY
#define DELETER_EMPTY [](auto){}
#endif

#ifndef COMP
#define COMP(T)
#endif

#define ENUM_INT(x) (std::to_underlying(x))

#if defined(_WIN32) || defined(_WIN64)
    #ifndef API_EXPORT
    #define API_EXPORT extern "C" __declspec(dllexport)
    #endif
#elif defined(__CNUC__) || defined(__clang__)
    #ifndef API_EXPORT
    #define API_EXPORT extern "C" __attribute__((visibility("default")))
    #endif
#else
    #ifndef API_EXPORT
    #define API_EXPORT extern "C"
    #endif
#endif

namespace Sierra {
	template <typename T>
    struct WeakPtrObjectHash {
        std::size_t operator()(const std::weak_ptr<T> weakPtr) const noexcept {
            return std::hash<std::shared_ptr<T>>{}(weakPtr.lock());
        }
    };
    
    template <typename T>
    struct WeakPtrObjectEquals {
        std::size_t operator()(const std::weak_ptr<T> lhs, const std::weak_ptr<T> rhs) const noexcept {
            return !lhs.owner_before(rhs) && !rhs.owner_before(lhs);
        }
    };
}