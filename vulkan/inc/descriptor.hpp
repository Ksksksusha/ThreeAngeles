#ifndef DESCRIPTOR_HPP_
#define DESCRIPTOR_HPP_

#include "device.hpp"
#include "swap_chain.hpp"
#include "uniform_buffer.hpp"


namespace Vulkan 
{

//-------------------------------------------------------------------------------//

class DescriptorSetLayout 
{
    public:

        DescriptorSetLayout(Device& device);
        
        ~DescriptorSetLayout();

        DescriptorSetLayout(const DescriptorSetLayout&)             = delete;
        
        DescriptorSetLayout &operator=(const DescriptorSetLayout&)  = delete;

        const VkDescriptorSetLayout& getDescriptorSetLayout_() const { return descriptorSetLayout_; }
        
    private:
        
        VkDescriptorSetLayout   descriptorSetLayout_;
        
        Device&                 device_;

};

//-------------------------------------------------------------------------------//

class DescriptorPool 
{
    public:
        
        DescriptorPool(Device& device, SwapChain& swapChain);
        
        ~DescriptorPool();

        DescriptorPool(const DescriptorPool&)               = delete;
        
        DescriptorPool &operator=(const DescriptorPool&)    = delete;

        const VkDescriptorPool& getDescriptorPool() const { return descriptorPool_; }


    private:
        
        VkDescriptorPool    descriptorPool_;

        Device&             device_;
        
        SwapChain&          swapChain_;
};

//-------------------------------------------------------------------------------//

class DescriptorSets
{
    public:
        
        DescriptorSets(Device& device, SwapChain& swapChain, UniformBuffer& uniformBuffer_, DescriptorSetLayout& descripterSetLayout, DescriptorPool& descripterPool);

        DescriptorSets(const DescriptorSets&)               = delete;
        
        DescriptorSets &operator=(const DescriptorSets&)    = delete;

        const std::vector<VkDescriptorSet>& getDescriptorSets() const { return descriptorSets_; }

    private:
        
        std::vector<VkDescriptorSet>    descriptorSets_;
        
        Device&                         device_;
        
        SwapChain&                      swapChain_;
        
        UniformBuffer&                  uniformBuffer_;

        DescriptorSetLayout&            descriptorSetLayout_;
        
        DescriptorPool&                 descriptorPool_;
};

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace

#endif