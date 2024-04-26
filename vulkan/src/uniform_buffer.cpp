#include "uniform_buffer.hpp"
#include "camera.hpp"
#include "window.hpp"

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

UniformBuffer::UniformBuffer(Device& device, SwapChain& swapChain, Camera& camera, Window& window) : device_(device), swapChain_(swapChain), camera_(camera), window_(window) 
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    uniformBuffers_.resize(swapChain.MAX_FRAMES_IN_FLIGHT);
    
    uniformBuffersMemory_.resize(swapChain.MAX_FRAMES_IN_FLIGHT);
    
    uniformBuffersMapped_.resize(swapChain.MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < swapChain.MAX_FRAMES_IN_FLIGHT; i++) 
    {
        device.createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers_[i], uniformBuffersMemory_[i]);

        vkMapMemory(device_.getDevice(), uniformBuffersMemory_[i], 0, bufferSize, 0, &uniformBuffersMapped_[i]);
    }
}

//-------------------------------------------------------------------------------//

UniformBuffer::~UniformBuffer() 
{
    for (size_t i = 0; i < swapChain_.MAX_FRAMES_IN_FLIGHT; i++) 
    {
        vkDestroyBuffer(device_.getDevice(), uniformBuffers_[i], nullptr);

        vkFreeMemory(device_.getDevice(), uniformBuffersMemory_[i], nullptr);
    }
}

//-------------------------------------------------------------------------------//

void UniformBuffer::update(GLFWwindow* const window, const uint32_t currentImage, const float frame_time) 
{
    camera_.determineMove(window, frame_time);
    
    camera_.determineRotate(window, frame_time);

    camera_.updateViewMatrix();

    UniformBuffer::UniformBufferObject ubo{};
    
    ubo.model = glm::mat4(1.0f);
    
    ubo.view = camera_.getVeiwMatrix();
    
    ubo.proj = glm::perspective(glm::radians(45.0f), swapChain_.getExtent().width / (float) swapChain_.getExtent().height, 0.1f, 10000.0f);
    
    ubo.viewPos = camera_.getPosition();
    
    ubo.proj[1][1] *= -1;

    memcpy(uniformBuffersMapped_[currentImage], &ubo, sizeof(ubo));
}

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace