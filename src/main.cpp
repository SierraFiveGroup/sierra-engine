#include "vk/vulkan.hpp"
#include "window/window.hpp"

using namespace Sierra;
using namespace vlk;

int main() {    
    
    Window window = Window("hehe", {1280, 720});
    Vulkan vulkan = Vulkan(window);
    vlk::Scene scene = vulkan.createScene();

}