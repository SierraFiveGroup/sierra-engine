#pragma once

#include <stdexcept>
#include <string>

#include "vulkan/vulkan.h"

#define STRINGIFY(x) #x
#define STRINGIFY_HELP(x) STRINGIFY(x)
#define VK_ERR(x) {VkResult ret{}; if((ret = x) != VK_SUCCESS) {throw std::runtime_error("Error check " #x " on " __FILE__ ":" STRINGIFY_HELP(__LINE__) " failed with code " + std::to_string(ret));}}
#define VK_ASSERT(x) if (!(x)) {throw std::runtime_error("Assert " #x " failed");}
