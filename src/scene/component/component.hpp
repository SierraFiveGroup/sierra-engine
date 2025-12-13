#pragma once

#include <cstdint>

//"we love casting spells"
#define SIERRA_GEN_COMP(x) \
\
extern "C" void init(void* ptr) {\
    *(x*)ptr = x();\
}\
\
extern "C" void destruct(void* ptr) {\
    ((x*)ptr)->~x();\
}\
\
extern "C" unsigned int getSize() {\
    return sizeof(x);\
}\

#define SIERRA_COMP(x) \
using namespace Sierra;\
class x : Component { \
static uint32_t _INTERNAL_blockOffset;\
static x* get(uint32_t objID) {\
    return nullptr;\
}

namespace Sierra{
    class Component {

    };
}