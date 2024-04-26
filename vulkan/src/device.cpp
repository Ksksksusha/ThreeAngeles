#include "device.hpp"
#include <iostream>

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

VkResult CreateDebugUtilsMessengerEXT(
        VkInstance instance,
        const VkDebugUtilsMessengerCreateInfoEXT *pCreateInfo,
        const VkAllocationCallbacks *pAllocator,
        VkDebugUtilsMessengerEXT *pDebugMessenger) 
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkCreateDebugUtilsMessengerEXT");
    
    if (func != nullptr) 
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    
    else 
        return VK_ERROR_EXTENSION_NOT_PRESENT;
}

//-------------------------------------------------------------------------------//

void DestroyDebugUtilsMessengerEXT(
        VkInstance instance,
        VkDebugUtilsMessengerEXT debugMessenger,
        const VkAllocationCallbacks *pAllocator) 
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance,
        "vkDestroyDebugUtilsMessengerEXT");
    
    if (func != nullptr) 
        func(instance, debugMessenger, pAllocator);
    
}

//-------------------------------------------------------------------------------//

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
    void *pUserData) 
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

//-------------------------------------------------------------------------------//

Device::Device(Window& window) : window_{window} 
{
    createInstance();
    
    setupDebugMessenger();
    
    createSurface();
    
    pickPhysicalDevice();
    
    createLogicalDevice();
    
    createCommandPool();
}

//-------------------------------------------------------------------------------//

Device::~Device() 
{
    vkDestroyCommandPool(device_, commandPool_, nullptr);

    vkDestroyDevice(device_, nullptr);

    if (enableValidationLayers) 
        DestroyDebugUtilsMessengerEXT(instance_, debugMessenger_, nullptr);

    vkDestroySurfaceKHR(instance_, surface_, nullptr);
    
    vkDestroyInstance(instance_, nullptr);
}

//-------------------------------------------------------------------------------//

void Device::createInstance() 
{
    if (enableValidationLayers && !checkValidationLayerSupport()) 
        throw std::runtime_error("validation layers requested, but not available!");

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "LittleVulkanEngine App";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    createInfo.flags |= VkInstanceCreateFlagBits::VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR; // important !

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    
    if (enableValidationLayers) 
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();

        populateDebugMessengerCreateInfo(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT *)&debugCreateInfo;
    }

    else 
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&createInfo, nullptr, &instance_) != VK_SUCCESS) 
        throw std::runtime_error("failed to create instance!");
}

//-------------------------------------------------------------------------------//

void Device::setupDebugMessenger() 
{
    if (!enableValidationLayers) 
        return;

    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    
    populateDebugMessengerCreateInfo(createInfo);

    if (CreateDebugUtilsMessengerEXT(instance_, &createInfo, nullptr, &debugMessenger_) != VK_SUCCESS)
        throw std::runtime_error("failed to set up debug messenger!");
}

//-------------------------------------------------------------------------------//

void Device::pickPhysicalDevice() 
{
    uint32_t deviceCount = 0;
    
    vkEnumeratePhysicalDevices(instance_, &deviceCount, nullptr);
    
    if (deviceCount == 0) 
        throw std::runtime_error("failed to find GPUs with Vulkan support!");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    
    vkEnumeratePhysicalDevices(instance_, &deviceCount, devices.data());

    physicalDevice_ = *(std::find_if(devices.begin(), devices.end(), [this] (VkPhysicalDevice device) {return isDeviceSuitable(device);}));

    if (physicalDevice_ == VK_NULL_HANDLE) 
        throw std::runtime_error("failed to find a suitable GPU!");
}

//-------------------------------------------------------------------------------//

void Device::createSurface() { window_.CreateWindowSurface(instance_, &surface_); }

//-------------------------------------------------------------------------------//

void Device::createLogicalDevice() 
{
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice_);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    
    for (uint32_t queueFamily : uniqueQueueFamilies) 
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures = {};
    deviceFeatures.samplerAnisotropy = VK_TRUE;

    VkDeviceCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (enableValidationLayers) 
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
        createInfo.ppEnabledLayerNames = validationLayers.data();
    } 
    
    else 
        createInfo.enabledLayerCount = 0;

    if (vkCreateDevice(physicalDevice_, &createInfo, nullptr, &device_) != VK_SUCCESS) 
        throw std::runtime_error("failed to create logical device!");

    vkGetDeviceQueue(device_, indices.graphicsFamily.value(), 0, &graphicsQueue_);

    vkGetDeviceQueue(device_, indices.presentFamily.value(), 0, &presentQueue_);
}

//-------------------------------------------------------------------------------//

void Device::createCommandPool() 
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(physicalDevice_);

    VkCommandPoolCreateInfo poolInfo = {};
    
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if (vkCreateCommandPool(device_, &poolInfo, nullptr, &commandPool_) != VK_SUCCESS)
        throw std::runtime_error("failed to create command pool!");
};

//-------------------------------------------------------------------------------//

bool Device::isDeviceSuitable(VkPhysicalDevice device) 
{
    QueueFamilyIndices indices = findQueueFamilies(device);

    bool extensionsSupported = checkDeviceExtensionSupport(device);

    bool swapChainAdequate = false;
    
    if (extensionsSupported) 
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
        
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    return indices.isComplete() && extensionsSupported && swapChainAdequate;
}

//-------------------------------------------------------------------------------//

bool Device::checkDeviceExtensionSupport(VkPhysicalDevice device) 
{
    uint32_t extensionCount = 0;
    
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    
    vkEnumerateDeviceExtensionProperties(
        device,
        nullptr,
        &extensionCount,
        availableExtensions.data());
    
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

    for (const auto& extension : availableExtensions)
        requiredExtensions.erase(extension.extensionName);

    return requiredExtensions.empty();
}

//-------------------------------------------------------------------------------//

QueueFamilyIndices Device::findQueueFamilies(VkPhysicalDevice device) 
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    
    for (const auto &queueFamily : queueFamilies) 
    {
        if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            indices.graphicsFamily = i;
            
        VkBool32 presentSupport = false;
        
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface_, &presentSupport);
        
        if (queueFamily.queueCount > 0 && presentSupport) 
            indices.presentFamily = i;
        
        if (indices.isComplete()) 
            break;

        i++;
    }

    return indices;
}

//-------------------------------------------------------------------------------//

SwapChainSupportDetails Device::querySwapChainSupport(VkPhysicalDevice device) 
{
    SwapChainSupportDetails details;
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface_, &details.capabilities);

    uint32_t formatCount;
    
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, nullptr);

    if (formatCount != 0) 
    {
        details.formats.resize(formatCount);
        
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface_, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface_, &presentModeCount, nullptr);

    if (presentModeCount != 0) 
    {
        details.presentModes.resize(presentModeCount);
        
        vkGetPhysicalDeviceSurfacePresentModesKHR(
            device,
            surface_,
            &presentModeCount,
            details.presentModes.data());
    }

    return details;
}

//-------------------------------------------------------------------------------//

bool Device::checkValidationLayerSupport() 
{
    uint32_t layerCount = 0;
    
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : validationLayers) 
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) 
        {
            if (strcmp(layerName, layerProperties.layerName) == 0) 
            {
                layerFound = true;
                
                break;
            }
        }

        if (!layerFound)
            return false;
    }

    return true;

    // return std::find_if(validationLayers.begin(), validationLayers.end(), [&availableLayers](auto&& layerName) {
    //     return std::find_if(availableLayers.begin(), availableLayers.end(), [&layerName](auto&& layer) {
    //         return std::strcmp(layer.layerName, layerName) == 0;
    //     }) != availableLayers.end();
    // }) != validationLayers.end();
}

//-------------------------------------------------------------------------------//

std::vector<const char*> Device::getRequiredExtensions() 
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
    extensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

    if (enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

//-------------------------------------------------------------------------------//

void Device::populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) 
{
    createInfo = {};
    
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    
    createInfo.pfnUserCallback = debugCallback;
    
    createInfo.pUserData = nullptr;
}

//-------------------------------------------------------------------------------//

uint32_t Device::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) 
{
    VkPhysicalDeviceMemoryProperties memProperties;
    
    vkGetPhysicalDeviceMemoryProperties(physicalDevice_, &memProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) 
    {
        if ((typeFilter & (1 << i)) &&
            (memProperties.memoryTypes[i].propertyFlags & properties) == properties) 
        {
            return i;
        }
    }

    throw std::runtime_error("failed to find suitable memory type!");
}

//-------------------------------------------------------------------------------//

VkFormat Device::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) 
{
    for (VkFormat format : candidates) 
    {
        VkFormatProperties props;
    
        vkGetPhysicalDeviceFormatProperties(physicalDevice_, format, &props);

    
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) 
            return format;
        
        else if (
            tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) 
        {
            return format;
        }
    }
    
    throw std::runtime_error("failed to find supported format!");
}

//-------------------------------------------------------------------------------//

void Device::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) 
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(device_, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) 
        throw std::runtime_error("failed to create vertex buffer!");

    VkMemoryRequirements memRequirements;
    
    vkGetBufferMemoryRequirements(device_, buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device_, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) 
        throw std::runtime_error("failed to allocate vertex buffer memory!");

    vkBindBufferMemory(device_, buffer, bufferMemory, 0);
}

//-------------------------------------------------------------------------------//

void Device::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool_;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    
    vkAllocateCommandBuffers(device_, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0;
    copyRegion.dstOffset = 0;
    copyRegion.size = size;
    
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue_, 1, &submitInfo, VK_NULL_HANDLE);
    
    vkQueueWaitIdle(graphicsQueue_);

    vkFreeCommandBuffers(device_, commandPool_, 1, &commandBuffer);
}

//-------------------------------------------------------------------------------//

void Device::createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) 
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType         = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType     = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width  = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth  = 1;
    imageInfo.mipLevels     = 1;
    imageInfo.arrayLayers   = 1;
    imageInfo.format        = format;
    imageInfo.tiling        = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage         = usage;
    imageInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags         = 0;

    if (vkCreateImage(device_, &imageInfo, nullptr, &image) != VK_SUCCESS) 
        throw std::runtime_error("failed to create image!");

    VkMemoryRequirements memRequirements;
    
    vkGetImageMemoryRequirements(device_, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

    if (vkAllocateMemory(device_, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) 
        throw std::runtime_error("failed to allocate image memory!");

    if (vkBindImageMemory(device_, image, imageMemory, 0) != VK_SUCCESS) 
        throw std::runtime_error("failed to bind image memory!");
}

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace