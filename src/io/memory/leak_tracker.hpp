#pragma once

#include <unordered_map>
#include <mutex>
#include <cstdlib>
#include <new>
#include <cstddef>
#include <string>

#include "../logging/logger.hpp"
#include "../../util/utilities.hpp"

#ifdef DEBUG || _DEBUG


namespace Sierra {   // optional — adjust or remove if you use a different namespace

    class LeakTracker {
    public:
        // ---- Public API ---------------------------------------------------
        static void init() {
            trackingEnabled() = false;   // disable recursion during setup
            getInstance();               // force singleton construction
            trackingEnabled() = true;    // enable tracking
            LOG("LeakTracker initialized");
        }
    
        static void shutdown() {
            getInstance().reportLeaks("Shutdown leak report:");
            LOG("LeakTracker shutdown complete");
        }
    
        static void recordAlloc(void* ptr, size_t size) noexcept {
            if (!ptr || !trackingEnabled()) return;
            auto& inst = getInstance();
            std::lock_guard<std::mutex> lock(inst.mutex_);
            inst.allocations_[ptr] = size;
        }
    
        static void recordFree(void* ptr) noexcept {
            if (!ptr || !trackingEnabled()) return;
            auto& inst = getInstance();
            std::lock_guard<std::mutex> lock(inst.mutex_);
            inst.allocations_.erase(ptr);
        }
    
    private:
        // ---- Internal details ---------------------------------------------
        std::unordered_map<void*, size_t> allocations_;
        std::mutex mutex_;
    
        LeakTracker() {
            allocations_.reserve(256);  // prevent early allocations
            LOG("LeakTracker constructed");
        }
    
        ~LeakTracker() {
            reportLeaks("Automatic shutdown leak report:");
        }
    
        void reportLeaks(const char* msg) noexcept {
            std::lock_guard<std::mutex> lock(mutex_);
            WARN(msg);
            if (allocations_.empty()) {
                LOG("No leaks detected.");
            } else {
                WARN("Outstanding allocations: " + std::to_string(allocations_.size()));
                for (auto& [ptr, size] : allocations_) {
                    std::string leakMsg = "Leaked block @ " +
                                          std::to_string(reinterpret_cast<uintptr_t>(ptr)) +
                                          " (" + std::to_string(size) + " bytes)";
                    WARN(leakMsg);
                }
            }
        }
    
        static bool& trackingEnabled() {
            static bool enabled = false;
            return enabled;
        }
    
        static LeakTracker& getInstance() {
            static LeakTracker instance;
            return instance;
        }
    };
    
    } // namespace Sierra
    
    // ---- Global operator overrides must live outside the namespace -------
    void* operator new(size_t size) {
        void* ptr = std::malloc(size);
        if (!ptr) throw std::bad_alloc();
        Sierra::LeakTracker::recordAlloc(ptr, size);
        return ptr;
    }
    
    void operator delete(void* ptr) noexcept {
        Sierra::LeakTracker::recordFree(ptr);
        std::free(ptr);
    }
#else

namespace Sierra {
    class LeakTracker {
        public:
            static void init() {}
            static void shutdown() {}
    };
}

#endif