#ifndef RENDER_SYSTEM_HPP_
#define RENDER_SYSTEM_HPP_

#include "device.hpp"
#include "object.hpp"
#include "pipeline.hpp"
#include "camera.hpp"

#include <memory>
#include <vector>

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

struct FrameInfo {
    int             frameIndex;
    float           frameTime;
    VkCommandBuffer commandBuffer;
    Camera&         camera;
    VkDescriptorSet descriptorSet;
};

//-------------------------------------------------------------------------------//

class RenderSystem 
{
    public:
    
        RenderSystem(Device& device, VkRenderPass render_pass, VkDescriptorSetLayout descriptorSetLayout);
        
        ~RenderSystem();

        RenderSystem(const RenderSystem &) = delete;
        
        RenderSystem &operator=(const RenderSystem &) = delete;

        
        void RenderObjects(std::vector<Object>& objects, FrameInfo& frameInfo);

    private:
    
        void CreatePipelineLayout(VkDescriptorSetLayout descriptorSetLayout);
        
        void CreatePipeline(VkRenderPass render_pass);

        Device&                         device_;

        std::unique_ptr<Pipeline>       pipeline_;
        
        VkPipelineLayout                pipeline_layout_;
};

//-------------------------------------------------------------------------------//

}  // end of Vulkan namespace

#endif