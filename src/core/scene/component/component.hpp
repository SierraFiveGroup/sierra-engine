#pragma once

#include <cstdint>
#include <concepts>
#include <cstddef>

#define SIERRA_COMP_CODE(A, B, C, D) ((uint32_t)(A << 24 || B << 16 || C << 8 || D))
#define SIERRA_COMP_CODE_STR(A) ((uint32_t)(A[0] << 24 || A[1] << 16 || A[2] << 8 || A[3]))

//"we love casting spells"
#define SIERRA_GEN_COMP(x, y)\
using namespace Sierra;\
using namespace Sierra::Component;\
uint32_t x::_INTERNAL_blockOffset = 0;\
uint32_t x::_INTERNAL_code = y;\
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
extern "C" size_t getBlockOffset() {\
    return x::_INTERNAL_blockOffset;\
}\
extern "C" uint32_t getCode() {\
    return x::_INTERNAL_code;\
}\


#define SIERRA_COMP(x)\
namespace Sierra::Component {;\
class x : public ComponentBase { \
public: static uint32_t _INTERNAL_blockOffset; static uint32_t _INTERNAL_code; protected:

#define SIERRA_CORE_COMP(x)\
namespace Sierra::Component {;\
class x : public ComponentBase, ComponentCore { \
public: static uint32_t _INTERNAL_blockOffset; static uint32_t _INTERNAL_code; protected:\
friend class PrivateAccess;

#define END_SIERRA_COMP };}

namespace Sierra::Component{
    class ComponentCore{

    };

    template<class T, class U>
    concept Derived = std::is_base_of_v<U, T>;

    class PrivateAccess { // all INTERNAL/CORE components have dat struct that can be accesed with a template
        public:

            template<Derived<ComponentCore> T>
            static inline uint32_t getDatOffset() {
                return (uint32_t)(size_t)&(((T*)nullptr)->data);
            }
    };

    class ComponentBase {
        public:
            static uint32_t _INTERNAL_blockOffset; // sacrifices had to be m ade

            virtual void update() = 0;
            static uint32_t getID();
    };
}
