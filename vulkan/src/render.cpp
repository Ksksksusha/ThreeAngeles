#include "render.hpp"
#include "descriptor.hpp"
#include "model.hpp"
#include "pipeline.hpp"

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

Render::Render(Device& device, SwapChain& swapChain, Pipeline& pipeline, Model& model, DescriptorSets& descriptorSets)
    : device_(device), swapChain_(swapChain), pipeline_(pipeline), model_(model), descriptorSets_(descriptorSets) 
{
    commandBuffers_.resize(swapChain_.MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    
    allocInfo.sType               = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level               = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool         = device_.getCommandPool();
    allocInfo.commandBufferCount  = static_cast<uint32_t>(commandBuffers_.size());

    if (vkAllocateCommandBuffers(device_.getDevice(), &allocInfo, commandBuffers_.data()) !=
        VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

//-------------------------------------------------------------------------------//

void Render::record(VkCommandBuffer commandBuffer, uint32_t imageIndex) 
{
    VkCommandBufferBeginInfo beginInfo{};
    
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) 
        throw std::runtime_error("failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType        = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass   = swapChain_.getRenderPass();
    renderPassInfo.framebuffer  = swapChain_.getFramebuffers()[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChain_.getExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color            = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1].depthStencil     = {1.0f, 0};
    renderPassInfo.clearValueCount  = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues     = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_.getGraphicsPipeline());

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swapChain_.getExtent().width);
    viewport.height = static_cast<float>(swapChain_.getExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, swapChain_.getExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    VkBuffer vertexBuffers[] = {model_.getVertexBuffer()};
    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, model_.getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_.getPipelineLayout(), 0, 1, &descriptorSets_.getDescriptorSets()[currentFrame_], 0, nullptr);
    vkCmdDrawIndexed(commandBuffer, static_cast<uint32_t>(model_.indices_.size()), 1, 0, 0, 0);
    
    vkCmdEndRenderPass(commandBuffer);
    
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("failed to record command buffer!");
}

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace