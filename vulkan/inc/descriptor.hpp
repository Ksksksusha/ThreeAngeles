#ifndef DESCRIPTOR_HPP_
#define DESCRIPTOR_HPP_

#include <memory>
#include <unordered_map>
#include <vector>

#include "device.hpp"

namespace Vulkan
{

//-------------------------------------------------------------------------------//

class DescriptorSetLayout 
{
    public:
    

        class Builder 
        {
            public:
                
                Builder(Device &device) : device{device} {}

                Builder& addBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t count = 1);

                std::unique_ptr<DescriptorSetLayout> build() const
                {
                    return std::make_unique<DescriptorSetLayout>(device, bindings);
                }

            private:
                
                Device &device;
                
                std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };



    public:

        DescriptorSetLayout(Device& device, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        
        ~DescriptorSetLayout() 
        {
            vkDestroyDescriptorSetLayout(device.device(), descriptorSetLayout, nullptr);
        }      
        
        DescriptorSetLayout(const DescriptorSetLayout &) = delete;
        
        DescriptorSetLayout &operator=(const DescriptorSetLayout &) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> getBindings() const { return bindings; }

    private:
        
        Device &device;
        
        VkDescriptorSetLayout descriptorSetLayout;
        
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

};

//-------------------------------------------------------------------------------//

class DescriptorPool 
{
    public:
        
        class Builder 
        {
            public:
                Builder(Device &device) : device{device} {}

                Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count) 
                {
                    poolSizes.push_back({descriptorType, count});
                    return *this;
                }
                
                Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags) 
                {
                    poolFlags = flags;
                    return *this;
                }
                
                Builder &setMaxSets(uint32_t count) 
                {
                    maxSets = count;
                    return *this;
                }
                
                std::unique_ptr<DescriptorPool> build() const 
                {
                    return std::make_unique<DescriptorPool>(device, maxSets, poolFlags, poolSizes);
                }

            private:
                
                Device &device;
                
                std::vector<VkDescriptorPoolSize> poolSizes{};
                
                uint32_t maxSets = 1000;
                
                VkDescriptorPoolCreateFlags poolFlags = 0;
        };

    public:
        
        DescriptorPool(Device &device, uint32_t maxSets, VkDescriptorPoolCreateFlags poolFlags, const std::vector<VkDescriptorPoolSize> &poolSizes);
    
        ~DescriptorPool() 
        {
            vkDestroyDescriptorPool(device.device(), descriptorPool, nullptr);
        }
        
        DescriptorPool(const DescriptorPool &) = delete;
        
        DescriptorPool &operator=(const DescriptorPool &) = delete;

        bool allocateDescriptor(const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;
        
        void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const 
        {
            vkFreeDescriptorSets(device.device(), descriptorPool, static_cast<uint32_t>(descriptors.size()), descriptors.data());
        }

        void resetPool() 
        {
            vkResetDescriptorPool(device.device(), descriptorPool, 0);
        }

        VkDescriptorPool getPool() const { return descriptorPool; }

        VkDevice getDevice() const { return device.device(); }

    private:
        Device &device;
        
        VkDescriptorPool descriptorPool;

};

//-------------------------------------------------------------------------------//

class DescriptorWriter 
{
    public:
       
        DescriptorWriter(DescriptorSetLayout &setLayout, DescriptorPool &pool)
            : setLayout{setLayout}, pool{pool} {}

        DescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);

        DescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

        bool build(VkDescriptorSet &set);
        
        void overwrite(VkDescriptorSet &set);

    private:
        
        DescriptorSetLayout &setLayout;
        
        DescriptorPool &pool;
        
        std::vector<VkWriteDescriptorSet> writes;
};

//-------------------------------------------------------------------------------//

}  // end of Vulkan namespace

#endif