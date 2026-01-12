#pragma once

#include <memory>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <cstring>

#include "tasks/task.hpp"

#include "res/model/model.hpp"
#include "res/texture/texture.hpp"

// too small to be worth splitting into multiple files
namespace Sierra::Res {
    union ResourceAny;
    typedef uint64_t ResID;

    enum class Type{
        TEXTURE,
        MODEL
    };

    struct Resource {
        bool (*isLoaded)(ResourceAny);        

        std::shared_ptr<void> extraDat; //internal classes and such
        ResID id;
        Type type;
    };

    struct Texture : Resource {
        enum class Type{
            COLOR 
        } texType;

    };

    struct Model : Resource {
        //std::unordered_map<Texture::Type, Texture> textures; VERY UNSAFE (how to handle?)
    };

    union ResourceAny {
        Texture texture;
        Model model;
        Resource base;

        ResourceAny() {
            memset(this, 0, sizeof(*this));
        }

        ResourceAny(ResourceAny& other) { // THE LION DOES NOT CONCERN HIMSELF WITH IMPROPER MOVE SEMANTICS
            memset(this, 0, sizeof(*this));
            copy(&other);
        }

        void operator=(ResourceAny& other) {
            copy(&other);
        }

        ResourceAny(ResourceAny&& other) {
            memset(this, 0, sizeof(*this));
            copy(&other);
        }

        void operator=(ResourceAny&& other) {
            copy(&other);
        }
        

        ~ResourceAny() {
            if(base.extraDat)
                base.extraDat.~__shared_ptr(); //vscode is lying this line is not an error
        }

        private:

            void copy(ResourceAny* other) {
                const size_t afterPtrOffset = offsetof(Resource, extraDat) + sizeof(base.extraDat);
                const size_t afterPtrSize = sizeof(*this) - afterPtrOffset;

                memcpy(this, other, offsetof(Resource, extraDat));
                if(base.extraDat)
                    base.extraDat.~__shared_ptr();
                new (&base.extraDat) std::shared_ptr<void>(other->base.extraDat);
                memcpy((uint8_t*)this + afterPtrOffset, (uint8_t*)other + afterPtrOffset, afterPtrSize);
            }
    };
}