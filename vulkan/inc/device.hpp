#ifndef DEVICE_HPP_
#define DEVICE_HPP_

#include "window.hpp"
#include <vector>
#include <string>
#include <optional>
#include <set>
#include <iostream>
#include <array>
#include <algorithm>
#include <cstring>

#define NDEBUG

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR        capabilities;
    
    std::vector<VkSurfaceFormatKHR> formats;
    
    std::vector<VkPresentModeKHR>   presentModes;
};


struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    
    std::optional<uint32_t> presentFamily;

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};

//-------------------------------------------------------------------------------//

class Device 
{
    public:

        Device(Window& window);
        
        ~Device();

        Device(const Device&) = delete;
        
        Device& operator=(const Device&) = delete;
        
        Device(const Device&&) = delete;
        
        Device& operator=(const Device&&) = delete;

        const VkCommandPool& getCommandPool()       const { return commandPool_; }
        
        const VkDevice& getDevice()                 const { return device_; }
        
        const VkSurfaceKHR& getSurface()            const { return surface_; }
        
        const VkQueue& getGraphicsQueue()           const { return graphicsQueue_; }
        
        const VkQueue& getPresentQueue()            const { return presentQueue_; }
        
        const VkPhysicalDevice& getPhysicalDevice() const { return physicalDevice_; }
        
        uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
        
        VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
        
        void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
        
        void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
        
        void createImage(uint32_t width, uint32_t height,
                        VkFormat format, VkImageTiling tiling,
                        VkImageUsageFlags usage,
                        VkMemoryPropertyFlags properties,
                        VkImage& image,
                        VkDeviceMemory& imageMemory);


        QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
        
        SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    

    private:

        void createInstance();
        
        void setupDebugMessenger();
        
        void createSurface();
        
        void pickPhysicalDevice();
        
        void createLogicalDevice();
        
        void createCommandPool();

        bool checkValidationLayerSupport();
        
        std::vector<const char *> getRequiredExtensions();
        
        void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
        
        bool isDeviceSuitable(VkPhysicalDevice device);
        
        bool checkDeviceExtensionSupport(VkPhysicalDevice device);


        Window& window_;
        
        VkInstance instance_;
        
        VkDevice device_;
        
        VkCommandPool commandPool_;
        
        VkSurfaceKHR surface_;
        
        VkQueue graphicsQueue_;
        
        VkQueue presentQueue_;

        VkDebugUtilsMessengerEXT debugMessenger_;
        
        VkPhysicalDevice physicalDevice_ = nullptr;

    #ifdef NDEBUG
        const std::array<const char*, 0> validationLayers = {};
    #else
        const std::array<const char*, 1> validationLayers = {"VK_LAYER_KHRONOS_validation"};
    #endif
            
        const std::array<const char*, 1> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

};

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace

#endif