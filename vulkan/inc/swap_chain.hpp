#ifndef SWAP_CHAIN_HPP_
#define SWAP_CHAIN_HPP_

#include "device.hpp"
#include "window.hpp"

#include <memory>
#include <string>
#include <vector>
#include <limits>

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

class SwapChain 
{
    public:
        
        const int MAX_FRAMES_IN_FLIGHT = 2;

        SwapChain(Window& window, Device& device);

        ~SwapChain();
        
        void cleanup();
        
        const VkSwapchainKHR& getSwapChain()                            const { return swapChain_; }
        
        const VkRenderPass& getRenderPass()                             const { return renderPass_; }
        
        const std::vector<VkFramebuffer>& getFramebuffers()             const { return swapChainFramebuffers_; }
        
        const VkExtent2D getExtent()                                    const { return swapChainExtent_; }
        
        const std::vector<VkSemaphore> getImageAvailableSemaphores()    const { return imageAvailableSemaphores_; }
        
        const std::vector<VkSemaphore> getRenderFinishedSemaphores()    const { return renderFinishedSemaphores_; }
        
        const std::vector<VkFence>     getInFlightFences()              const { return inFlightFences_; }
        
        void recreate();

        SwapChain(const SwapChain&) = delete;
        
        SwapChain&operator=(const SwapChain&) = delete;

    private:

        void createSwapChain();
        
        void createImageViews();
        
        void createRenderPass();
        
        void createDepthResources();
        
        void createFramebuffers();
        
        void createSyncObjects();

        VkSurfaceFormatKHR          chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
        
        VkPresentModeKHR            chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
        
        VkExtent2D                  chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

        VkImageView                 createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
        
        VkFormat                    findDepthFormat();
    
        Window&                     window_;
        
        Device&                     device_;
        
        VkExtent2D                  swapChainExtent_;

        VkSwapchainKHR              swapChain_;
        
        std::vector<VkImage>        swapChainImages_;
        
        VkFormat                    swapChainImageFormat_;
        
        std::vector<VkImageView>    swapChainImageViews_;
        
        std::vector<VkFramebuffer>  swapChainFramebuffers_;
        
        VkRenderPass                renderPass_;

        VkImage                     depthImage_;
        
        VkDeviceMemory              depthImageMemory_;
        
        VkImageView                 depthImageView_;

        std::vector<VkSemaphore>    imageAvailableSemaphores_;
        
        std::vector<VkSemaphore>    renderFinishedSemaphores_;
        
        std::vector<VkFence>        inFlightFences_;
};

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace

#endif