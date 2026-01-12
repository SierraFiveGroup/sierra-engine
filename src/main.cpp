#include <vk/vulkan.hpp>
#include "window/window.hpp"

#include "res/texture/texture.hpp"

#include "util/structures/hive.hpp"

#include "vk/res/shader/descriptor/descriptor_set.hpp"
#include "vk/rendering/graphics_pipeline/pipeline_layout.hpp"
#include "vk/rendering/graphics_pipeline/graphics_pipeline.hpp"
#include "vk/res/command_buffer/command_pool.hpp"
#include "vk/res/command_buffer/command_buffer.hpp"
#include "vk/res/mem/buffer/buffer.hpp"
#include "vk/res/mem/image/image.hpp"
#include "vk/res/mem/mem_loader.hpp"
#include "vk/sync/fence/fence.hpp"
#include "vk/res/loadable/model/vk_model.hpp"
#include "vk/res/loadable/texture/vk_texture.hpp"

#include "tasks/manager.hpp"
#include "scene/loader/loadable_resource.hpp"

#include "io/panic/panic_handler.hpp"
#include "io/memory/leak_tracker.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <scene/component/loader/component_loader.hpp>

#include "engine.hpp"

#include <unordered_set>

#include "common/util/structures/hive.hpp"

using namespace Sierra;
using namespace vlk;
int main() {
    Engine engine = Engine();


    /*
    putenv("LD_LIBRARY_PATH=components/so"); // put in engine::init or smthn
    ComponentTemplate comp = ComponentLoader::loadComponent("libfoo.so");
    comp.init(
        malloc(sizeof(Foo))
    );

    //set up panic handler
    //PanicHandler::init();

    //set up leak tracking
    //LeakTracker::init();

    //std::cout << "Not hanging :)\n";
    
    Window window = Window("hehe", {1280, 720}, API::vulkan);
    Vulkan vulkan = Vulkan(window);;

    TaskManager taskManager = TaskManager();
    MemLoader loader = MemLoader(vulkan.getContext());
    MemoryManager memManager = MemoryManager(vulkan.getContext());

    uint8_t dat[] = {1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5, 1, 2, 3, 4, 5};

    std::future<Buffer> devBuff = loader.createBuff(memManager, Buffer::Type::DEVICE_LOCAL, Buffer::Usage::VERTEX, dat, sizeof(dat));

    MemLoader::ImageInfo imageInfo{};
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.extent = {5, 1, 1};
    imageInfo.layout = VK_IMAGE_LAYOUT_GENERAL;
    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;

    std::future<Image> img = loader.createImage(memManager, imageInfo, dat, sizeof(dat));

    //glm::mat4 proj = glm::perspective(45.0f, 1.67f, 0.0f, 1.0f);
    glm::mat4 proj = glm::ortho(-2.0f, 2.0f, 2.0f, -2.0f);

    std::future<Buffer> projBuffFuture =
        loader.createBuff(memManager, Buffer::Type::DEVICE_LOCAL, Buffer::Usage::UNIFORM, (uint8_t*)glm::value_ptr(proj), 64);

    Model model = Model(taskManager, "models/Orangutan/Orangutan/Orangutan.obj");
    VlkModel vlkModel = VlkModel(vulkan.getContext(), taskManager, memManager, loader, model);

    Texture texture = Texture(taskManager, "lepotec.jpg");

    taskManager.addTasks(memManager.getTasks());
    taskManager.printTasks();
    taskManager.start();

    while(!taskManager.isFinished());

    vlk::Scene scene = vulkan.createScene();
    vlk::Shader shaderVert = vlk::Shader(vulkan.getContext(), "test_shaders/vk/model.vert.spv", "test_shaders/vk/model.vert");
    vlk::Shader shaderFrag = vlk::Shader(vulkan.getContext(), "test_shaders/vk/model.frag.spv", "test_shaders/vk/model.frag");

    auto descriptors = shaderVert.getDescriptors();
    descriptors.insert(descriptors.end(), shaderFrag.getDescriptors().begin(), shaderFrag.getDescriptors().end());

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

    VkSubpassDependency extDependency{};
    extDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    extDependency.dstSubpass = 0;
    extDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
    extDependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    extDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    extDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    
    vlk::RenderPass::Info renderPassInfo{};
    renderPassInfo.attachments = {attachmentDescription};
    renderPassInfo.dependencies = {extDependency};
    renderPassInfo.subpasses = {subpass};
    renderPassInfo.imageViews = vulkan.getSwapchain().getImageViews();
    renderPassInfo.width = window.getResolution().w;
    renderPassInfo.height = window.getResolution().h;
    renderPassInfo.hasDepth = true;
    

    vlk::RenderPass renderPass = vlk::RenderPass(vulkan.getContext(), memManager, renderPassInfo);

    VkVertexInputBindingDescription bindingDescription1{};
    bindingDescription1.binding = 0;
    bindingDescription1.stride = 12;
    bindingDescription1.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputBindingDescription bindingDescription2{};
    bindingDescription2.binding = 1;
    bindingDescription2.stride = 12;
    bindingDescription2.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attributeDescription1{};
    attributeDescription1.location = 0;
    attributeDescription1.binding = 0;
    attributeDescription1.format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription1.offset = 0;

    VkVertexInputAttributeDescription attributeDescription2{};
    attributeDescription2.location = 1;
    attributeDescription2.binding = 1;
    attributeDescription2.format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription2.offset = 0;

    VkVertexInputAttributeDescription attributeDescriptions[] = {attributeDescription1, attributeDescription2};
    VkVertexInputBindingDescription bindingDescriptions[] = {bindingDescription1, bindingDescription2};

    VkPipelineVertexInputStateCreateInfo inputState{};
    inputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    inputState.pVertexAttributeDescriptions = attributeDescriptions;
    inputState.pVertexBindingDescriptions = bindingDescriptions;
    inputState.vertexAttributeDescriptionCount = 2;
    inputState.vertexBindingDescriptionCount = 2;


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

    while(!taskManager.isFinished());
    
    CommandPool cmdPool = CommandPool(vulkan.getContext(), vulkan.getContext().device->getQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT), VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
    CommandBuffer drawCmdBuffer = CommandBuffer(cmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY);


    VkViewport viewport{};
    viewport.width = window.getResolution().w;
    viewport.height = window.getResolution().h;
    viewport.minDepth = 0.0;
    viewport.maxDepth = 1.0;

    //NOTE depth clear value should be 1 not 0

    VkRect2D scissor{};
    scissor = {{0, 0}, {window.getResolution().w, window.getResolution().h}};

    Fence fence = Fence(vulkan.getContext());

    uint32_t imgIndex = 0;

    Buffer projBuff = projBuffFuture.get();
    VkWriteDescriptorSet descriptorWrite[] = {
        descriptors[0].getWriteBuffer(projBuff),
        descriptors[1].getWriteImage(
            *vlkModel.getTexture(aiTextureType_DIFFUSE)
        )
    };

    VkDescriptorSet setHandle = set.getSet();

    vkUpdateDescriptorSets(vulkan.getContext().device->getDevice(), 2, descriptorWrite, 0, nullptr);

    DBG(vlkModel.getMeshes()[0].vertexBuffOffset);
    DBG(vlkModel.getMeshes()[0].texCoordBuffOffset);
    while(!window.shouldClose()) {

        vkAcquireNextImageKHR(vulkan.getContext().device->getDevice(), 
            vulkan.getSwapchain().getSwapchain(), -1, VK_NULL_HANDLE, fence.getFence(),
            &imgIndex);

        VkRenderPassBeginInfo passBegin = renderPass.getBeginInfo(imgIndex);

        VkFence fenceHandle = fence.getFence();
        vkWaitForFences(vulkan.getContext().device->getDevice(), 1, &fenceHandle, VK_TRUE, -1);


        VkBuffer vertexBuffs[] = {vlkModel.getVertexBuffer().getBuff(),
            vlkModel.getVertexBuffer().getBuff()};
        VkDeviceSize offsets[] = {
            0, vlkModel.getMeshes()[0].texCoordBuffOffset
        };

        VkBuffer indexBuff = vlkModel.getIndexBuffer().getBuff();

        drawCmdBuffer.begin(nullptr);
        vkCmdSetViewport(drawCmdBuffer.getCommandBuffer(), 0, 1, &viewport);
        vkCmdSetScissor(drawCmdBuffer.getCommandBuffer(), 0, 1, &scissor);

        
        vkCmdBindPipeline(drawCmdBuffer.getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        vkCmdBeginRenderPass(drawCmdBuffer.getCommandBuffer(), &passBegin, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindVertexBuffers(drawCmdBuffer.getCommandBuffer(), 0, 2, vertexBuffs, offsets);
        vkCmdBindIndexBuffer(drawCmdBuffer.getCommandBuffer(), indexBuff, 0, VK_INDEX_TYPE_UINT32);
        vkCmdBindDescriptorSets(drawCmdBuffer.getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, 
            layout, 0, 1, &setHandle, 0, nullptr);
        vkCmdDrawIndexed(drawCmdBuffer.getCommandBuffer(), vlkModel.getIndexCount(), 1, 0, 0, 0);
//        vkCmdDraw(drawCmdBuffer.getCommandBuffer(), vlkModel.getVertexCount(), 1, 0, 0);
        vkCmdEndRenderPass(drawCmdBuffer.getCommandBuffer());
        drawCmdBuffer.end();

        VkCommandBuffer commandBufferHandle = drawCmdBuffer.getCommandBuffer();

        VkSubmitInfo queueSubmit{};
        queueSubmit.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        queueSubmit.commandBufferCount = 1;
        queueSubmit.pCommandBuffers = &commandBufferHandle;

        vkResetFences(vulkan.getContext().device->getDevice(), 1, &fenceHandle);

        vkQueueSubmit(vulkan.getContext().device->getQueue(VK_QUEUE_GRAPHICS_BIT), 1, &queueSubmit, fence.getFence());

        vkWaitForFences(vulkan.getContext().device->getDevice(), 1, &fenceHandle, VK_TRUE, -1);

        VkSwapchainKHR swapchainHandle = vulkan.getSwapchain().getSwapchain();

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.pSwapchains = &swapchainHandle;
        presentInfo.swapchainCount = 1;
        presentInfo.pImageIndices = &imgIndex;

        vkQueuePresentKHR(vulkan.getContext().device->getQueue(VK_QUEUE_GRAPHICS_BIT), &presentInfo);

        vkResetFences(vulkan.getContext().device->getDevice(), 1, &fenceHandle);
        drawCmdBuffer.reset();

        glfwPollEvents();

        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // SLEEP DON'T FORGET
    }

    vlk::DescriptorLayout::destroy(vulkan.getContext());
    vlk::GraphicsPipeline::destroy(vulkan.getContext());
    vlk::PipelineLayout::destroy(vulkan.getContext());


    //LeakTracker::shutdown();
*/
}