#pragma once

namespace Sierra{
    struct ComponentTemplate {
        void* dlptr;

        std::string name;

        void* (*init)(void*);
        void (*destruct)(void* obj);
        size_t (*getSize)();
        void (*setBlockOffset)(size_t);
        size_t (*getBlockOffset)();
    };
}