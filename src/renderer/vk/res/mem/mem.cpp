#include "mem.hpp"

#define VMA_IMPLEMENTATION /*sum bullshit*/
#define VMA_sTATIC_VULKAN_FUNCTIONS 1 // if yall dont have this god help you
#include "vk_mem_alloc.h"

namespace Sierra::vlk {
    static std::unordered_map<VkDevice, std::shared_ptr<VmaAllocator>> allocators; 
    ResourceRef<VmaAllocator> getAllocator(Context& context) {
        auto it = allocators.find(context.device->getDevice());
        if (it == allocators.end()) {
            Mem::init(context);
            return getAllocator(context);
        }

        return it->second;
    }

    void Mem::init(Context& context) {
        if (allocators.find(context.device->getDevice()) != allocators.end()) {
            WARN("Trying to create a preexistent VmaAllocator with device " << context.device->getDevice());
            return;
        }

        VmaAllocator allocator = (VmaAllocator)createAllocator(context);
        allocators[context.device->getDevice()] = std::make_shared<VmaAllocator>(allocator);
    }

    void Mem::destroy(Context& context) {
        auto it = allocators.find(context.device->getDevice());

        if (it == allocators.end()) {
            WARN("Trying to destroy a nonexistent VmaAllocator with device " << context.device->getDevice());
            return;
        }

        vmaDestroyAllocator(*it->second);
        allocators.erase(it);
    }

    VmaAllocator Mem::createAllocator(Context& context) { //TODO fetch the fucking extensions
        VmaAllocatorCreateInfo allocInfo{};
        //allocInfo.flags = ...
        allocInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        allocInfo.physicalDevice = context.device->getPhysicalDevice();
        allocInfo.device = context.device->getDevice();
        allocInfo.instance = context.instance->getInstance();

        VmaAllocator allocator;
        VK_ERR(vmaCreateAllocator(&allocInfo, &allocator));

        return allocator;
    }


    Mem::Mem(): allocator(), allocation(), buffer(), image(), allocInfo() {
        
    }

    Mem::Mem(Context& context, Info& info, VkBufferCreateInfo& bufferInfo, VkBuffer& buffer): allocator(), buffer(), image(), allocInfo() {
        allocator = getAllocator(context);
        createBuffer(info, bufferInfo, buffer);

        this->buffer = buffer;
    }

    Mem::Mem(Context& context, Info& info, VkImageCreateInfo& imageInfo, VkImage& image): buffer(), image(), allocInfo() {
        allocator = getAllocator(context);
        createImage(info, imageInfo, image);

        this->image = image;
    }

    void Mem::createBuffer(Info& info, VkBufferCreateInfo& bufferInfo, VkBuffer& buffer) {
        VmaAllocationCreateInfo allocInfo{};
        allocInfo.flags = info.flags;
        allocInfo.requiredFlags = info.required;
        allocInfo.preferredFlags = info.preferred;
        allocInfo.priority = info.priority;
        allocInfo.usage = (VmaMemoryUsage)info.type;

        VK_ERR(vmaCreateBuffer(*allocator.getLock(), &bufferInfo, &allocInfo, &buffer, &allocation, nullptr));
    }

    void Mem::createImage(Info& info, VkImageCreateInfo& imageInfo, VkImage& image) {
        VmaAllocationCreateInfo allocInfo{};
        allocInfo.flags = info.flags;
        allocInfo.requiredFlags = info.required;
        allocInfo.preferredFlags = info.preferred;
        allocInfo.priority = info.priority;
        allocInfo.usage = (VmaMemoryUsage)info.type;

        VK_ERR(vmaCreateImage(*allocator.getLock(), &imageInfo, &allocInfo, &image, &allocation, nullptr));
    }

    VmaAllocationInfo& Mem::getAllocInfo() {
        return allocInfo;
    }

    Mem::Mem(Mem&& other) {
        this->allocator = other.allocator;
        this->allocation = other.allocation;
        this->buffer = other.buffer;
        this->image = other.image;
        this->allocInfo = other.allocInfo;

        other.allocator = nullptr;
        other.allocation = nullptr;
        other.image = VK_NULL_HANDLE;
        other.buffer = VK_NULL_HANDLE;
        other.allocInfo = {0};
    }

    void Mem::operator=(Mem&& other) {
        this->allocator = other.allocator;
        this->allocation = other.allocation;
        this->buffer = other.buffer;
        this->image = other.image;
        this->allocInfo = other.allocInfo;

        other.allocator = nullptr;
        other.allocation = nullptr;
        other.image = VK_NULL_HANDLE;
        other.buffer = VK_NULL_HANDLE;
        other.allocInfo = {0};
    }

    Mem::~Mem() {
        if (buffer) 
            vmaDestroyBuffer(*allocator.getLock(), buffer, allocation);
        else if(image)
            vmaDestroyImage(*allocator.getLock(), image, allocation);
    }

    void* Mem::map() {
        void* ptr;
        vmaMapMemory(*allocator.getLock(), allocation, &ptr);

        return ptr;
    }

    void Mem::unmap() {
        vmaUnmapMemory(*allocator.getLock(), allocation);
    }

    void Mem::copyToHost(uint8_t* src, size_t srcSize) {
        vmaCopyMemoryToAllocation(*allocator.getLock(), src, allocation, 0, srcSize);
    }

    VkDeviceSize Mem::getSize() {
        return allocation->GetSize();
    }

    VkDeviceSize Mem::getOffset() {
        return allocation->GetOffset();
    }

    ///////

    MemPool::MemPool() {

    }

    MemPool::MemPool(Context& context, Info& info): allocator() {
        allocator = getAllocator(context);
    }

}