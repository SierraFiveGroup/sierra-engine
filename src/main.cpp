#include "vulkan/vulkan.hpp"
#include "window/window.hpp"

using namespace Sierra;
using namespace vk;

int main() {    
    
    Window window = Window("hehe", {1280, 720});
    Vulkan vulkan = Vulkan(window);
}