#ifndef PIPELINE_HPP_
#define PIPELINE_HPP_

#include "device.hpp"

#include <vector>
#include <string>
#include <iostream>

namespace Vulkan
{

//-------------------------------------------------------------------------------//

struct PipelineConfigInfo {
    PipelineConfigInfo() = default;
    
    PipelineConfigInfo(const PipelineConfigInfo&) = delete;
    
    PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

    VkPipelineViewportStateCreateInfo           viewportInfo;
    
    VkPipelineInputAssemblyStateCreateInfo      inputAssemblyInfo;
    
    VkPipelineRasterizationStateCreateInfo      rasterizationInfo;
    
    VkPipelineMultisampleStateCreateInfo        multisampleInfo;
    
    VkPipelineColorBlendAttachmentState         colorBlendAttachment;
    
    VkPipelineColorBlendStateCreateInfo         colorBlendInfo;
    
    VkPipelineDepthStencilStateCreateInfo       depthStencilInfo;

    std::vector<VkDynamicState>                 dynamicStateEnables;

    VkPipelineDynamicStateCreateInfo            dynamicStateInfo;
    
    VkPipelineLayout                            pipelineLayout = nullptr;
    
    VkRenderPass                                renderPass = nullptr;
    
    uint32_t subpass = 0;
};

//-------------------------------------------------------------------------------//

class Pipeline 
{
    public:
        
        Pipeline(
            Device& device,
            const std::string& vertFilepath,
            const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo);
        
        ~Pipeline();

        Pipeline(const Pipeline&) = delete;
        
        Pipeline& operator=(const Pipeline&) = delete;

        void Bind(VkCommandBuffer commandBuffer);

        static void DefaultPipelineConfigInfo(PipelineConfigInfo& configInfo);

    private:
        
        void CreateGraphicsPipeline(
            const std::string& vertFilepath,
            const std::string& fragFilepath,
            const PipelineConfigInfo& configInfo);

        void CreateShaderModule(const std::string& filePath, VkShaderModule* shaderModule);

        Device&         Device_;
        
        VkPipeline      graphicsPipeline_;
        
        VkShaderModule  vertShaderModule_;
        
        VkShaderModule  fragShaderModule_;
};

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace

#endif