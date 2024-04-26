#include "descriptor.hpp"
#include "device.hpp"
#include "uniform_buffer.hpp"

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

DescriptorSetLayout::DescriptorSetLayout(Device& device) : device_(device)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};

    uboLayoutBinding.binding            = 0;

    uboLayoutBinding.descriptorType     = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

    uboLayoutBinding.descriptorCount    = 1;

    uboLayoutBinding.stageFlags         = VK_SHADER_STAGE_ALL_GRAPHICS;

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{};
    
    descriptorSetLayoutInfo.sType           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    
    descriptorSetLayoutInfo.bindingCount    = 1;
    
    descriptorSetLayoutInfo.pBindings       = &uboLayoutBinding;

    if (vkCreateDescriptorSetLayout(
        device_.getDevice(),
        &descriptorSetLayoutInfo,
        nullptr,
        &descriptorSetLayout_) != VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

//-------------------------------------------------------------------------------//

DescriptorSetLayout::~DescriptorSetLayout() 
{
    vkDestroyDescriptorSetLayout(device_.getDevice(), descriptorSetLayout_, nullptr);
}

//-------------------------------------------------------------------------------//

DescriptorPool::DescriptorPool(Device& device, SwapChain& swapChain) 
    : device_(device), swapChain_(swapChain) 
{
    VkDescriptorPoolSize poolSize{
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        static_cast<uint32_t>(swapChain_.MAX_FRAMES_IN_FLIGHT)
    };

    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    
    descriptorPoolInfo.maxSets = static_cast<uint32_t>(swapChain_.MAX_FRAMES_IN_FLIGHT);
    
    descriptorPoolInfo.poolSizeCount = 1;
    
    descriptorPoolInfo.pPoolSizes = &poolSize;
    
    if (vkCreateDescriptorPool(device_.getDevice(), &descriptorPoolInfo, nullptr, &descriptorPool_) !=
        VK_SUCCESS) 
    {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

//-------------------------------------------------------------------------------//

DescriptorPool::~DescriptorPool() 
{
    vkDestroyDescriptorPool(device_.getDevice(), descriptorPool_, nullptr);
}

//-------------------------------------------------------------------------------//


DescriptorSets::DescriptorSets(Device& device, SwapChain& swapChain, UniformBuffer& uniformBuffer, DescriptorSetLayout& descripterSetLayout, DescriptorPool& descripterPool) :
    device_(device), swapChain_(swapChain), uniformBuffer_(uniformBuffer), descriptorSetLayout_(descripterSetLayout), descriptorPool_(descripterPool) 
{
    std::vector<VkDescriptorSetLayout> layouts(swapChain.MAX_FRAMES_IN_FLIGHT, descriptorSetLayout_.getDescriptorSetLayout_());

    VkDescriptorSetAllocateInfo allocInfo{};

    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    allocInfo.descriptorPool = descripterPool.getDescriptorPool();

    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain.MAX_FRAMES_IN_FLIGHT);

    allocInfo.pSetLayouts = layouts.data();

    descriptorSets_.resize(swapChain.MAX_FRAMES_IN_FLIGHT);

    if (vkAllocateDescriptorSets(device_.getDevice(), &allocInfo, descriptorSets_.data()) != VK_SUCCESS)
        throw std::runtime_error("failed to allocate descriptor sets!");
    
    for (size_t i = 0; i < swapChain.MAX_FRAMES_IN_FLIGHT; i++) 
    {
        VkDescriptorBufferInfo bufferInfo{
            uniformBuffer_.getUniformBuffers()[i],
            0,
            uniformBuffer_.getSizeOfUniformBufferObject()
        };

        VkWriteDescriptorSet descriptorWrite{};

        descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;

        descriptorWrite.dstSet = descriptorSets_[i];

        descriptorWrite.dstBinding = 0;

        descriptorWrite.dstArrayElement = 0;

        descriptorWrite.descriptorCount = 1;

        descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

        descriptorWrite.pImageInfo = {};

        descriptorWrite.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(device_.getDevice(), 1, &descriptorWrite, 0, nullptr);
    }
}

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace