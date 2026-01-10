#pragma once

#include <cstdint>

//"we love casting spells"
#define SIERRA_GEN_COMP(x) \
uint32_t x::_INTERNAL_blockOffset = 0;\
\
extern "C" void init(void* ptr) {\
   new((x*)ptr) x();\
}\
extern "C" void destruct(void* ptr) {\
    ((x*)ptr)->~x();\
}\
\
extern "C" unsigned int getSize() {\
    return sizeof(x);\
}\
extern "C" void setBlockOffset(size_t offset) {\
    x::_INTERNAL_blockOffset = offset;\
}\

#define SIERRA_COMP(x) \
using namespace Sierra;\
class x : Component { \
public:\
static uint32_t _INTERNAL_blockOffset;\
protected:\
static x* get(uint32_t objID);

namespace Sierra{
    class Component {
        public:
            virtual void update() = 0;
    };
}