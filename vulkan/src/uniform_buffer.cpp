#include "uniform_buffer.hpp"
 
#include <cassert>
#include <cstring>
#include <iostream>
 
namespace Vulkan 
{

//-------------------------------------------------------------------------------//
 
VkDeviceSize UniformBuffer::getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment) 
{
    if (minOffsetAlignment > 0) 
        return (instanceSize + minOffsetAlignment - 1) & ~(minOffsetAlignment - 1);

    return instanceSize;
}

//-------------------------------------------------------------------------------//
 
UniformBuffer::UniformBuffer(
    Device &device,
    VkDeviceSize instanceSize,
    uint32_t instanceCount,
    VkBufferUsageFlags usageFlags,
    VkMemoryPropertyFlags memoryPropertyFlags,
    VkDeviceSize minOffsetAlignment)
    : device_{device},
    instanceSize_{instanceSize},
    instanceCount_{instanceCount},
    usageFlags_{usageFlags},
    memoryPropertyFlags_{memoryPropertyFlags} 
{
    alignmentSize_ = getAlignment(instanceSize, minOffsetAlignment);
    
    bufferSize_ = alignmentSize_ * instanceCount;
    
    device_.createBuffer(bufferSize_, usageFlags, memoryPropertyFlags, buffer_, memory_);
}

//-------------------------------------------------------------------------------//
 
UniformBuffer::~UniformBuffer() 
{
    unmap();
    
    vkDestroyBuffer(device_.device(), buffer_, nullptr);
    
    vkFreeMemory(device_.device(), memory_, nullptr);
}
 
//-------------------------------------------------------------------------------//

VkResult UniformBuffer::map(VkDeviceSize size, VkDeviceSize offset)
{
    assert(buffer_ && memory_ && "Called map on buffer before create");

    std::cout << mapped_ << std::endl;
    
    return vkMapMemory(device_.device(), memory_, offset, size, 0, &mapped_);
}

//-------------------------------------------------------------------------------//
 
void UniformBuffer::unmap() 
{
    if (mapped_) 
    {
        vkUnmapMemory(device_.device(), memory_);
    
        mapped_ = nullptr;
    }
}

//-------------------------------------------------------------------------------//
 
void UniformBuffer::writeToBuffer(void *data, VkDeviceSize size, VkDeviceSize offset) 
{
    assert(mapped && "Cannot copy to unmapped buffer");
    
    if (size == VK_WHOLE_SIZE) 
        memcpy(mapped_, data, bufferSize_);
    
    else 
    {
        char *memOffset = (char *)mapped_;
    
        memOffset += offset;
    
        memcpy(memOffset, data, size);
    }
}
 
//-------------------------------------------------------------------------------//

VkResult UniformBuffer::flush(VkDeviceSize size, VkDeviceSize offset) 
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory_;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkFlushMappedMemoryRanges(device_.device(), 1, &mappedRange);
}

//-------------------------------------------------------------------------------//

VkResult UniformBuffer::invalidate(VkDeviceSize size, VkDeviceSize offset) 
{
    VkMappedMemoryRange mappedRange = {};
    mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
    mappedRange.memory = memory_;
    mappedRange.offset = offset;
    mappedRange.size = size;
    return vkInvalidateMappedMemoryRanges(device_.device(), 1, &mappedRange);
}

//-------------------------------------------------------------------------------//

VkDescriptorBufferInfo UniformBuffer::descriptorInfo(VkDeviceSize size, VkDeviceSize offset) 
{
    return VkDescriptorBufferInfo{
        buffer_,
        offset,
        size,
    };
}
 
//-------------------------------------------------------------------------------//

void UniformBuffer::writeToIndex(void *data, int index) 
{
    writeToBuffer(data, instanceSize_, index * alignmentSize_);
}
 
//-------------------------------------------------------------------------------//

VkResult UniformBuffer::flushIndex(int index) 
{
    return flush(alignmentSize_, index * alignmentSize_); 
}
 
//-------------------------------------------------------------------------------//

VkDescriptorBufferInfo UniformBuffer::descriptorInfoForIndex(int index) 
{
    return descriptorInfo(alignmentSize_, index * alignmentSize_);
}
 
//-------------------------------------------------------------------------------//

VkResult UniformBuffer::invalidateIndex(int index) 
{
    return invalidate(alignmentSize_, index * alignmentSize_);
}

//-------------------------------------------------------------------------------//
 
}  // end of Vulkan namespace 