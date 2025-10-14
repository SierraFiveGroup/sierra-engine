#pragma once

#include <memory>
#include <stdexcept>

#include "io/logging/logger.hpp"

namespace Sierra {

    template<typename T>
    class ResourceRef {
        public:
            ResourceRef() {
                hash = 0;
            }

            ResourceRef(std::weak_ptr<T> ptr): weakPtr(ptr), rawPtr(nullptr) {
                hash = std::hash<std::shared_ptr<T>>{}(weakPtr.lock());
            }

            ResourceRef(std::shared_ptr<T> ptr): weakPtr(ptr), rawPtr(nullptr) {
                hash = std::hash<std::shared_ptr<T>>{}(weakPtr.lock());
            }
            ResourceRef(T* ptr): rawPtr(ptr) {
                hash = (size_t)ptr;
            }

            T* operator->() const {
                if (isEmpty()) 
                    throw std::runtime_error("Tried to access an empty component");

                if (rawPtr)
                    return rawPtr;
                
                return weakPtr.lock().get();
            }

            bool isEmpty() const {
                return weakPtr.use_count() == 0 && rawPtr == nullptr;
            }

            std::shared_ptr<T> getLock() {
                if (isEmpty()) 
                    throw std::runtime_error("Tried to access an empty component");
                
                if (rawPtr) 
                    throw std::runtime_error("Tried to access a reference with a raw handle");
                

                return weakPtr.lock();
            }

            std::weak_ptr<T> getSmartPtr() {
                if (isEmpty()) 
                    throw std::runtime_error("Tried to access an empty component");
                
                if (rawPtr)
                    throw std::runtime_error("Tried to access a reference with a raw handle");

                return weakPtr;
            }

            T* getRawPtr() {
                if (isEmpty()) 
                    throw std::runtime_error("Tried to access an empty component");
                
                if (weakPtr.use_count() == 0)
                    throw std::runtime_error("Tried to access a reference with a smart ptr handle");

                return rawPtr;
            }

            bool operator==(const ResourceRef &other) const noexcept {
                return hash == other.hash;
            }

            size_t getHash() const {
                if (hash == 0) {
                    WARN("Tried to hash an empty ResourceRef container");
                }

                return hash;
            }

            template<typename G>
            operator ResourceRef<G>() {
                return ResourceRef<G>(
                    std::static_pointer_cast<G>(weakPtr.lock()) // reinterpret_cast?? NO
                );
            }

            void reset() {
                hash = 0;
                rawPtr = nullptr;
                weakPtr.reset();
            }

        private:
            std::weak_ptr<T> weakPtr;
            T* rawPtr;

            size_t hash;
    };
}

namespace std {
    template<typename T>
    struct hash<Sierra::ResourceRef<T>> {
        auto operator()(const Sierra::ResourceRef<T>& ref) const -> size_t {
            return ref.getHash();
        }
    };
}