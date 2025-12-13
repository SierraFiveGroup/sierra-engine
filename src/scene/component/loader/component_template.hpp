#pragma once

namespace Sierra{
    struct ComponentTemplate {
        void* dlptr;

        void* (*init)(void*);
        void (*update)();
        void (*destruct)(void* obj);
    };
}