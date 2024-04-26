#ifndef UNIFORM_BUFFER_HPP_
#define UNIFORM_BUFFER_HPP_

#include "device.hpp"
#include "swap_chain.hpp"
#include "camera.hpp"
#include "window.hpp"

#include <glm/glm.hpp>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

class UniformBuffer 
{
    public:

        struct UniformBufferObject {
                glm::mat4 model;
                glm::mat4 view;
                glm::mat4 proj;
                glm::vec3 viewPos;
            };

        UniformBuffer(Device& device, SwapChain& swapChain, Camera& camera, Window& window);
        
        ~UniformBuffer();

        UniformBuffer(const UniformBuffer&)             = delete;
        
        UniformBuffer& operator=(const UniformBuffer&)  = delete;

        const std::vector<VkBuffer>& getUniformBuffers()    const { return uniformBuffers_; }
        
        auto getSizeOfUniformBufferObject()                 const { return sizeof(UniformBufferObject); }
        
        const std::vector<void*>& getUniformBeffersMapped() const { return uniformBuffersMapped_; }

        void update(GLFWwindow* const window, const uint32_t currentImage, const float frame_time);

    private:
        
        Device&                         device_;
        
        SwapChain&                      swapChain_;
        
        Camera&                         camera_;
        
        Window&                         window_;

        std::vector<VkBuffer>           uniformBuffers_;
        
        std::vector<VkDeviceMemory>     uniformBuffersMemory_;
        
        std::vector<void*>              uniformBuffersMapped_;
    
};

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace

#endif