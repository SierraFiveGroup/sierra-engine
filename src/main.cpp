#include "vk/vulkan.hpp"
#include "window/window.hpp"

#include "vk/res/shader/descriptor/descriptor_set.hpp"
#include "vk/rendering/graphics_pipeline/pipeline_layout.hpp"
#include "vk/rendering/graphics_pipeline/graphics_pipeline.hpp"
#include "vk/res/command_buffer/command_pool.hpp"
#include "vk/res/command_buffer/command_buffer.hpp"
#include "vk/res/mem/buffer/buffer.hpp"
#include "vk/sync/fence/fence.hpp"

#include "scene/tasks/manager.hpp"
#include "scene/loader/loadable_resource.hpp"

using namespace Sierra;
using namespace vlk;

void f1(std::shared_ptr<void>, std::function<void()> f) {
    std::cerr << "1\n";
    f();
}

void f2(std::shared_ptr<void>, std::function<void()> f) {
    std::cerr << "2\n";
    f();
}

void f3(std::shared_ptr<void>, std::function<void()> f) {
    std::cerr << "3\n";
    f();
}

int main() {    
    
    try{
    /*
    Window window = Window("hehe", {1280, 720});
    Vulkan vulkan = Vulkan(window);

    std::vector<uint32_t> transferFamilyIndex = {vulkan.getContext().device->getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT)};
    char testDat[] = {1, 2, 3};

    Buffer::Info buffInfo{};
    buffInfo.queueFamilyIndices = transferFamilyIndex;
    buffInfo.size = sizeof(testDat);
    buffInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    buffInfo.type = Buffer::Type::HOST_LOCAL;

    Buffer stagingBuff = Buffer(vulkan.getContext(), buffInfo);

    buffInfo.type = Buffer::Type::DEVICE_LOCAL;
    Buffer buff = Buffer(vulkan.getContext(), buffInfo);

    CommandPool pool = CommandPool(vulkan.getContext(), vulkan.getContext().device->getQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT), 0);
    CommandBuffer cmdBuff = CommandBuffer(pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);

    stagingBuff.copyToBuff((uint8_t*)testDat, sizeof(testDat));

    VkBufferCopy copyReg{};
    copyReg.size = sizeof(testDat);

    cmdBuff.begin(nullptr);
    vkCmdCopyBuffer(cmdBuff.getCommandBuffer(), stagingBuff.getBuff(), buff.getBuff(), 1, &copyReg);
    cmdBuff.end();

    VkCommandBuffer cmdBuffRaw = cmdBuff.getCommandBuffer();

    Fence fence = Fence(vulkan.getContext());

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &cmdBuffRaw;

    VkFence fenceRaw = fence.getFence();

    VK_ERR(vkQueueSubmit(vulkan.getContext().device->getQueue(VK_QUEUE_TRANSFER_BIT), 1, &submitInfo, fence.getFence()));
    VK_ERR(vkWaitForFences(vulkan.getContext().device->getDevice(), 1, &fenceRaw, VK_TRUE, (uint64_t)-1));*/


    TaskManager manager = TaskManager();
    LoadableResource res = LoadableResource("LICENSE");
    manager.addTask(res.getTask());
    
    
    manager.start();
    std::cout << (char*)res.getDat().get().data() << "\n";

    while(!manager.isFinished());

    } catch (std::exception e) {
        std::cerr << e.what() << "\n";
    }

   /* vlk::Scene scene = vulkan.createScene();
    vlk::Shader shaderVert = vlk::Shader(vulkan.getContext(), "test_shaders/vk/tri.vert.spv", "test_shaders/vk/tri.vert");
    vlk::Shader shaderFrag = vlk::Shader(vulkan.getContext(), "test_shaders/vk/tri.frag.spv", "test_shaders/vk/tri.frag");

    auto descriptors = shaderVert.getDescriptors();

    std::array<size_t, SIERRA_VLK_DESCRIPTOR_TYPE_COUNT> sizes = {};
    std::vector<Descriptor*> descriptorPtrs = {};

    for(auto& descriptor : descriptors) {
        sizes[descriptor.getType()]++;
        descriptorPtrs.push_back(&descriptor);
    }

    vlk::DescriptorPool pool = vlk::DescriptorPool(vulkan.getContext(), sizes);

    vlk::DescriptorSet set = vlk::DescriptorSet(vulkan.getContext(), descriptorPtrs, pool);

    std::vector<VkPushConstantRange> ranges{};
    VkPipelineLayout layout = vlk::PipelineLayout::getLayout(vulkan.getContext(), set.getLayout(), ranges);

    VkAttachmentDescription attachmentDescription{};
    attachmentDescription.format = vulkan.getSwapchain().getSurfaceFormat().format;
    attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    vlk::RenderPass::CreateInfo renderPassInfo{};
    renderPassInfo.attachments = {attachmentDescription};
    renderPassInfo.dependencies = {};
    renderPassInfo.subpasses = {subpass};


    vlk::RenderPass renderPass = vlk::RenderPass(vulkan.getContext(), renderPassInfo);

    VkPipelineVertexInputStateCreateInfo inputState{};
    inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkPipelineShaderStageCreateInfo vertexStage{};
    vertexStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexStage.module = shaderVert.getShader();
    vertexStage.pName = "main";
    vertexStage.stage = VK_SHADER_STAGE_VERTEX_BIT;

    VkPipelineShaderStageCreateInfo fragStage{};
    fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStage.module = shaderFrag.getShader();
    fragStage.pName = "main";
    fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;

    vlk::GraphicsPipeline::PipelineInfo pipelineInfo = {};
    pipelineInfo.renderPass = &renderPass;
    pipelineInfo.basePipeline = VK_NULL_HANDLE;
    pipelineInfo.inputState = inputState;
    pipelineInfo.layout = layout;
    pipelineInfo.subpass = 0;
    pipelineInfo.stages = {vertexStage, fragStage};
    
    VkPipeline pipeline = vlk::GraphicsPipeline::getPipeline(vulkan.getContext(), pipelineInfo);

    vlk::DescriptorLayout::destroy(vulkan.getContext());
    vlk::GraphicsPipeline::destroy(vulkan.getContext());
    vlk::PipelineLayout::destroy(vulkan.getContext());*/


}