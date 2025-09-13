#include "vk/vulkan.hpp"
#include "window/window.hpp"

#include "vk/res/shader/descriptor/descriptor_set.hpp"

using namespace Sierra;
using namespace vlk;

int main() {    
    
    Window window = Window("hehe", {1280, 720});
    Vulkan vulkan = Vulkan(window);
    vlk::Scene scene = vulkan.createScene();
    vlk::Shader shader = vlk::Shader(vulkan.getContext(), "test_shaders/vk/tri.vert.spv", "test_shaders/vk/tri.vert");

    auto descriptors = shader.getDescriptors();

    std::array<size_t, SIERRA_VLK_DESCRIPTOR_TYPE_COUNT> sizes;
    std::vector<Descriptor*> descriptorPtrs;

    for(auto& descriptor : descriptors) {
        sizes[descriptor.getType()]++;
        descriptorPtrs.push_back(&descriptor);
    }

    vlk::DescriptorPool pool = vlk::DescriptorPool(vulkan.getContext(), sizes);

    vlk::DescriptorSet set = vlk::DescriptorSet(vulkan.getContext(), descriptorPtrs, pool);

    vlk::DescriptorLayout::destroy(vulkan.getContext());

}