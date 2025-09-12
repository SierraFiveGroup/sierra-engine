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

    auto sizes = shader.getDescriptorSizes();
    std::vector<VkDescriptorSetLayoutBinding> bindings{};

    for(int i = 0; i < bindings.size(); i++) {
        if(!sizes[i]) continue;

        VkDescriptorSetLayoutBinding binding{};
        binding.binding = i;
        binding.descriptorType = (VkDescriptorType)i;
        binding.descriptorCount = sizes[i];
        binding.stageFlags = VK_SHADER_STAGE_ALL;

        bindings.push_back(binding);
    }

    vlk::DescriptorPool pool = vlk::DescriptorPool(vulkan.getContext(), sizes);

    vlk::DescriptorSet set = vlk::DescriptorSet(vulkan.getContext(), bindings, pool);

    vlk::DescriptorLayout::destroy(vulkan.getContext());

}