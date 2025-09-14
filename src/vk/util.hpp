#pragma once

#include <stdexcept>

#include "vulkan/vulkan.h"

#define STRINGIFY(x) #x
#define VK_ERR(x) if(x != VK_SUCCESS) {throw new std::runtime_error("Error check " #x " on " __FILE__ ":" STRINGIFY(__LINE__) " failed with code " STRINGIFY(x));}
#define VK_ASSERT(x) if (!(x)) {throw new std::runtime_error("Assert " #x " failed");}
