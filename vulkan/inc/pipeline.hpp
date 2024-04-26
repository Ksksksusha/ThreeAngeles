#pragma once

#include "descriptor.hpp"
#include "device.hpp"
#include "model.hpp"
#include "swap_chain.hpp"

#include <string>
#include <fstream>


namespace Vulkan 
{

//-------------------------------------------------------------------------------//

class Pipeline 
{
    public:

        Pipeline(Device& device, SwapChain& swapChain, DescriptorSetLayout& descriptorSetLayout, const std::string& vertFilepath, const std::string& fragFilepath);
        
        ~Pipeline();

        Pipeline& operator=(const Pipeline&) = delete;
        
        Pipeline(const Pipeline&) = delete;

        const VkPipelineLayout& getPipelineLayout()     const { return pipelineLayout_; }
        
        const VkPipeline&       getGraphicsPipeline()   const { return graphicsPipeline_; }

    private:
        
        Device&                 device_;
        
        SwapChain&              swapChain_;
        
        DescriptorSetLayout&    descriptorSetLayout_;
        
        std::string             vertFilepath_;
        
        std::string             fragFilepath_;

        VkPipelineLayout        pipelineLayout_;
        
        VkPipeline              graphicsPipeline_;

        VkShaderModule          vertShaderModule_;
        
        VkShaderModule          fragShaderModule_;
        
        static std::vector<char> readFile(const std::string& filename);
        
        void createShaderModule(const std::string& filePath, VkShaderModule* shaderModule); 
};

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace