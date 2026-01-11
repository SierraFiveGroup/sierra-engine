#pragma once

#include <memory>
#include <atomic>
#include <vector>
#include <unordered_map>
#include <cstring>

#include "tasks/task.hpp"

// too small to be worth splitting into multiple files
namespace Sierra::Res {
    union ResourceAny;

    struct Resource {
        enum class Type{
            TEXTURE,
            MODEL
        };

        bool (*isLoaded)(ResourceAny);        

        std::shared_ptr<uint8_t> extraDat; //internal classes and such
        uint64_t id;
        Type type;
    };

    struct Texture : Resource {
        enum class Type{
            COLOR 
        } type;

    };

    struct Model : Resource {
        std::unordered_map<Texture::Type, Texture> textures;
    };

    union ResourceAny {
        Texture texture;
        Model model;
        Resource base;

        ResourceAny() {
            memset(this, 0, sizeof(*this));
        }

        ResourceAny(ResourceAny& other) {
            memcpy(&other, this, sizeof(*this));
        }

        void operator=(ResourceAny& other) {
            memcpy(&other, this, sizeof(*this));
        }

        ResourceAny(ResourceAny&& other) {
            memcpy(&other, this, sizeof(*this));
        }

        void operator=(ResourceAny&& other) {
            memcpy(&other, this, sizeof(*this));
        }

        ~ResourceAny() {
            base.extraDat.~__shared_ptr(); //vscode is lying this line is not an error
        }
    };
}