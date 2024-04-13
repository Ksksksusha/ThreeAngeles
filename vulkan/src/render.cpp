#include "render.hpp"

#include <array>
#include <stdexcept>
#include <cassert>

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

Render::Render(Window& window, Device& device) : window_{window}, device_{device} 
{
    RecreateSwapChain();
    
    CreateCommandBuffers();
}

//-------------------------------------------------------------------------------//

Render::~Render() 
{ 
    FreeCommandBuffers();
}

//-------------------------------------------------------------------------------//

void Render::RecreateSwapChain()
{
    auto extent = window_.GetExtent();

    while (extent.width == 0 || extent.height == 0)
    {
        extent = window_.GetExtent();

        glfwWaitEvents();
    }

    vkDeviceWaitIdle(device_.device());

    if (swap_chain_ == nullptr)
        swap_chain_ = std::make_unique<SwapChain>(device_, extent);

    else
    {
        std::shared_ptr<SwapChain> old_swap_chain = std::move(swap_chain_);

        swap_chain_ = std::make_unique<SwapChain>(device_, extent, old_swap_chain);
        
        if (swap_chain_->ImageCount() != command_buffers_.size())
        {
            FreeCommandBuffers();

            CreateCommandBuffers();
        }
    }
}

//-------------------------------------------------------------------------------//

void Render::CreateCommandBuffers() 
{
    command_buffers_.resize(swap_chain_->ImageCount());

    VkCommandBufferAllocateInfo allocInfo{};
    
    allocInfo.sType               = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level               = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool         = device_.getCommandPool();
    allocInfo.commandBufferCount  = static_cast<uint32_t>(command_buffers_.size());

    if (vkAllocateCommandBuffers(device_.device(), &allocInfo, command_buffers_.data()) !=
        VK_SUCCESS) 
    {
        throw std::runtime_error("failed to allocate command buffers!");
    }
}

//-------------------------------------------------------------------------------//

void Render::FreeCommandBuffers()
{
    vkFreeCommandBuffers(
        device_.device(), 
        device_.getCommandPool(),
        static_cast<float>(command_buffers_.size()),
        command_buffers_.data());

    command_buffers_.clear();
}

//-------------------------------------------------------------------------------//

VkCommandBuffer Render::BeginFrame()
{
    assert(!is_frame_started_);

    auto result = swap_chain_->AcquireNextImage(&current_image_index_);

    if (result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();

        return nullptr;
    }

    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) 
        throw std::runtime_error("failed to acquire swap chain image!");

    is_frame_started_ = true;

    auto command_buffer = GetCurrentCommandBuffer();

    VkCommandBufferBeginInfo beginInfo{};
    
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(command_buffer, &beginInfo) != VK_SUCCESS) 
        throw std::runtime_error("failed to begin recording command buffer!");

    return command_buffer;
}

//-------------------------------------------------------------------------------//

void Render::EndFrame()
{
    assert(is_frame_started_);

    auto command_buffer = GetCurrentCommandBuffer();

    if (vkEndCommandBuffer(command_buffer) != VK_SUCCESS) 
        throw std::runtime_error("failed to record command buffer!");

    auto result = swap_chain_->SubmitCommandBuffers(&command_buffer, &current_image_index_);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window_.WasWindowResized())
    {
        window_.ResetWindowResizeFlag();

        RecreateSwapChain();
    }

    else if (result != VK_SUCCESS) 
        throw std::runtime_error("failed to present swap chain image!");

    is_frame_started_ = false;
}

//-------------------------------------------------------------------------------//

void Render::BeginSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    assert(is_frame_started_);
    assert(commandBuffer == GetCurrentCommandBuffer());

    VkRenderPassBeginInfo renderPassInfo{};

    renderPassInfo.sType        = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass   = swap_chain_->GetRenderPass();
    renderPassInfo.framebuffer  = swap_chain_->GetFrameBuffer(current_image_index_);

    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swap_chain_->GetSwapChainExtent();

    std::array<VkClearValue, 2> clearValues{};
    
    clearValues[0].color            = {0.0f, 0.0f, 0.0f, 1.0f};
    clearValues[1].depthStencil     = {1.0f, 0};
    renderPassInfo.clearValueCount  = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues     = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(swap_chain_->GetSwapChainExtent().width);
    viewport.height = static_cast<float>(swap_chain_->GetSwapChainExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor{{0, 0}, swap_chain_->GetSwapChainExtent()};
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

//-------------------------------------------------------------------------------//

void Render::EndSwapChainRenderPass(VkCommandBuffer commandBuffer)
{
    assert(is_frame_started_);
    assert(commandBuffer == GetCurrentCommandBuffer());

    vkCmdEndRenderPass(commandBuffer);
}

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace