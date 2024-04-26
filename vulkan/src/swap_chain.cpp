#include "swap_chain.hpp"
#include "device.hpp"

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

SwapChain::SwapChain(Window& window, Device& device) : window_(window), device_(device) 
{
    createSwapChain();
    
    createImageViews();
    
    createRenderPass();
    
    createDepthResources();
    
    createFramebuffers();
    
    createSyncObjects();
};

//-------------------------------------------------------------------------------//

SwapChain::~SwapChain() 
{
    cleanup();

    vkDestroyRenderPass(device_.getDevice(), renderPass_, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {
        vkDestroySemaphore(device_.getDevice(), renderFinishedSemaphores_[i], nullptr);

        vkDestroySemaphore(device_.getDevice(), imageAvailableSemaphores_[i], nullptr);

        vkDestroyFence(device_.getDevice(), inFlightFences_[i], nullptr);
    }
}

//-------------------------------------------------------------------------------//

void SwapChain::cleanup() 
{
    vkDestroyImageView(device_.getDevice(), depthImageView_, nullptr);

    vkDestroyImage(device_.getDevice(), depthImage_, nullptr);

    vkFreeMemory(device_.getDevice(), depthImageMemory_, nullptr);

    for (size_t i = 0; i < swapChainFramebuffers_.size(); i++)
        vkDestroyFramebuffer(device_.getDevice(), swapChainFramebuffers_[i], nullptr);

    for (size_t i = 0; i < swapChainImageViews_.size(); i++)
        vkDestroyImageView(device_.getDevice(), swapChainImageViews_[i], nullptr);

    vkDestroySwapchainKHR(device_.getDevice(), swapChain_, nullptr);
}

//-------------------------------------------------------------------------------//

void SwapChain::createSwapChain() 
{
    SwapChainSupportDetails swapChainSupport = device_.querySwapChainSupport(device_.getPhysicalDevice());

    VkSurfaceFormatKHR surfaceFormat  = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode      = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent                 = chooseSwapExtent(swapChainSupport.capabilities);

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 &&
        imageCount > swapChainSupport.capabilities.maxImageCount) 
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType    = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface  = device_.getSurface();

    createInfo.minImageCount    = imageCount;
    createInfo.imageFormat      = surfaceFormat.format;
    createInfo.imageColorSpace  = surfaceFormat.colorSpace;
    createInfo.imageExtent      = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices      = device_.findQueueFamilies(device_.getPhysicalDevice());
    uint32_t queueFamilyIndices[]   = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) 
    {
        createInfo.imageSharingMode         = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount    = 2;
        createInfo.pQueueFamilyIndices      = queueFamilyIndices;
    } 
    
    else 
    {
        createInfo.imageSharingMode         = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount    = 0;    
        createInfo.pQueueFamilyIndices      = nullptr;
    }

    createInfo.preTransform     = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode  = presentMode;
    createInfo.clipped      = VK_TRUE;

    createInfo.oldSwapchain = nullptr;

    if (vkCreateSwapchainKHR(device_.getDevice(), &createInfo, nullptr, &swapChain_) != VK_SUCCESS) 
        throw std::runtime_error("failed to create swap chain!");

    vkGetSwapchainImagesKHR(device_.getDevice(), swapChain_, &imageCount, nullptr);
    
    swapChainImages_.resize(imageCount);
    
    vkGetSwapchainImagesKHR(device_.getDevice(), swapChain_, &imageCount, swapChainImages_.data());

    swapChainImageFormat_ = surfaceFormat.format;
    
    swapChainExtent_ = extent;
}

//-------------------------------------------------------------------------------//

void SwapChain::createImageViews() 
{
    swapChainImageViews_.resize(swapChainImages_.size());
    
    for (size_t i = 0; i < swapChainImages_.size(); i++) 
    {
        VkImageViewCreateInfo viewInfo{};
        
        viewInfo.sType                              = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        
        viewInfo.image                              = swapChainImages_[i];
        
        viewInfo.viewType                           = VK_IMAGE_VIEW_TYPE_2D;
        
        viewInfo.format                             = swapChainImageFormat_;
        
        viewInfo.subresourceRange.aspectMask        = VK_IMAGE_ASPECT_COLOR_BIT;
        
        viewInfo.subresourceRange.baseMipLevel      = 0;
        
        viewInfo.subresourceRange.levelCount        = 1;
        
        viewInfo.subresourceRange.baseArrayLayer    = 0;
        
        viewInfo.subresourceRange.layerCount        = 1;

        if (vkCreateImageView(device_.getDevice(), &viewInfo, nullptr, &swapChainImageViews_[i]) !=
            VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create texture image view!");
        }
    }
}

//-------------------------------------------------------------------------------//

void SwapChain::createRenderPass() 
{
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format            = findDepthFormat();
    depthAttachment.samples           = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp            = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp           = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp     = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp    = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout       = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment   = {};
    colorAttachment.format                    = swapChainImageFormat_;
    colorAttachment.samples                   = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp                    = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp                   = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp            = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp             = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.initialLayout             = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout               = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef  = {};
    colorAttachmentRef.attachment             = 0;
    colorAttachmentRef.layout                 = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass      = {};
    subpass.pipelineBindPoint         = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount      = 1;
    subpass.pColorAttachments         = &colorAttachmentRef;
    subpass.pDepthStencilAttachment   = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.dstSubpass          = 0;
    dependency.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask       = 0;
    dependency.srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    
    VkRenderPassCreateInfo renderPassInfo   = {};
    renderPassInfo.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount          = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments             = attachments.data();
    renderPassInfo.subpassCount             = 1;
    renderPassInfo.pSubpasses               = &subpass;
    renderPassInfo.dependencyCount          = 1;
    renderPassInfo.pDependencies            = &dependency;

    if (vkCreateRenderPass(device_.getDevice(), &renderPassInfo, nullptr, &renderPass_) != VK_SUCCESS) 
        throw std::runtime_error("failed to create render pass!");
}

//-------------------------------------------------------------------------------//

void SwapChain::createDepthResources() 
{
    VkFormat depthFormat = findDepthFormat();
    
    device_.createImage(swapChainExtent_.width, swapChainExtent_.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage_, depthImageMemory_);
    
    depthImageView_ = createImageView(depthImage_, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

//-------------------------------------------------------------------------------//

void SwapChain::createFramebuffers() 
{
    swapChainFramebuffers_.resize(swapChainImageViews_.size());
    
    for (size_t i = 0; i < swapChainImageViews_.size(); i++) 
    {
        std::array<VkImageView, 2> attachments = {swapChainImageViews_[i], depthImageView_};

        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass              = renderPass_;
        framebufferInfo.attachmentCount         = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments            = attachments.data();
        framebufferInfo.width                   = swapChainExtent_.width;
        framebufferInfo.height                  = swapChainExtent_.height;
        framebufferInfo.layers                  = 1;

        if (vkCreateFramebuffer(
                device_.getDevice(),
                &framebufferInfo,
                nullptr,
                &swapChainFramebuffers_[i]) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

//-------------------------------------------------------------------------------//

void SwapChain::createSyncObjects() 
{
    imageAvailableSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    
    renderFinishedSemaphores_.resize(MAX_FRAMES_IN_FLIGHT);
    
    inFlightFences_.resize(MAX_FRAMES_IN_FLIGHT);
    
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {
        if (vkCreateSemaphore(device_.getDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]) !=
                VK_SUCCESS ||
            vkCreateSemaphore(device_.getDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]) !=
                VK_SUCCESS ||
            vkCreateFence(device_.getDevice(), &fenceInfo, nullptr, &inFlightFences_[i]) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create synchronization objects for a frame!");
        }
    }
}

//-------------------------------------------------------------------------------//

VkFormat SwapChain::findDepthFormat() 
{
  return device_.findSupportedFormat(
      {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
      VK_IMAGE_TILING_OPTIMAL,
      VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}

//-------------------------------------------------------------------------------//

VkImageView SwapChain::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags) 
{
    VkImageViewCreateInfo viewInfo{};
        
    viewInfo.sType                              = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        
    viewInfo.image                              = image;
        
    viewInfo.viewType                           = VK_IMAGE_VIEW_TYPE_2D;
        
    viewInfo.format                             = format;
        
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    
    viewInfo.subresourceRange.baseMipLevel = 0;
    
    viewInfo.subresourceRange.levelCount = 1;
    
    viewInfo.subresourceRange.baseArrayLayer = 0;
    
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView{};

    if (vkCreateImageView(device_.getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS)
        throw std::runtime_error("failed to create texture image view!");

    return imageView;
}

//-------------------------------------------------------------------------------//

VkSurfaceFormatKHR SwapChain::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) 
{
    for (const auto &availableFormat : availableFormats) 
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
        {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

//-------------------------------------------------------------------------------//

VkPresentModeKHR SwapChain::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) 
{
    for (const auto &availablePresentMode : availablePresentModes) 
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
        {
            std::cout << "Present mode: Mailbox" << std::endl;
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

//-------------------------------------------------------------------------------//

VkExtent2D SwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) 
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
        return capabilities.currentExtent;
   
    else 
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window_.GetGLFWwindow(), &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width  = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

//-------------------------------------------------------------------------------//

void SwapChain::recreate() 
{
    int width = 0, height = 0;
    
    glfwGetFramebufferSize(window_.GetGLFWwindow(), &width, &height);
    
    while (width == 0 || height == 0)
    {
        glfwGetFramebufferSize(window_.GetGLFWwindow(), &width, &height);
        glfwWaitEvents();
    
    }

    vkDeviceWaitIdle(device_.getDevice());
    
    cleanup();

    createSwapChain();
    
    createImageViews();
    
    createDepthResources();
    
    createFramebuffers();
}

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace