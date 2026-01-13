#pragma once

#include <cstdint>

//"we love casting spells"
#define SIERRA_GEN_COMP(x) \
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
class x : public Component { \

namespace Sierra{
    class Component {
        public:
            static uint32_t _INTERNAL_blockOffset; // sacrifices had to be m ade

            virtual void update() = 0;
            static uint32_t getID();
    };
}