#ifndef RENDER_HPP_
#define RENDER_HPP_

#include <vector>

#include "device.hpp"
#include "pipeline.hpp"
#include "swap_chain.hpp"
#include "descriptor.hpp"
#include "camera.hpp"

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

class Render 
{
    public:
        
        uint32_t currentFrame_ = 0;
        
        Render(Device& device, SwapChain& swapChain, Pipeline& pipeline, Model& model, DescriptorSets& descriptorSets);

        Render(const Render&) = delete;
        
        Render& operator=(const Render&) = delete;

        const std::vector<VkCommandBuffer>& getCommandBuffers() const { return commandBuffers_; }

        void record(VkCommandBuffer commandBuffer, uint32_t imageIndex);
  
    private:
        
        std::vector<VkCommandBuffer>    commandBuffers_;
        
        Device&                         device_;
        
        SwapChain&                      swapChain_;
        
        Pipeline&                       pipeline_;
        
        Model&                          model_;
        
        DescriptorSets&                 descriptorSets_;
};

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace

#endif