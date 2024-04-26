#include "window.hpp"
#include <stdexcept>

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

Window::Window(uint32_t width, uint32_t height, std::string windowName) 
    : width_(width), height_(height), windowName_(windowName) 
{ 
    InitWindow();
}

//-------------------------------------------------------------------------------//

void Window::InitWindow() 
{
    glfwInit();
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window_ = glfwCreateWindow(width_, height_, windowName_.c_str(), nullptr, nullptr);
    
    glfwSetWindowUserPointer(window_, this);
    
    glfwSetFramebufferSizeCallback(window_, FramebufferResizeCallback);
}

//-------------------------------------------------------------------------------//

void Window::FramebufferResizeCallback(GLFWwindow* window, int width, int height) 
{
    auto vulkan_window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
    
    vulkan_window->SetFrameBufferRisizedFlag();
    
    vulkan_window->width_  = width;
    
    vulkan_window->height_ = height;
}

//-------------------------------------------------------------------------------//

void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR * surface) 
{
    if (glfwCreateWindowSurface(instance, window_, nullptr, surface) != VK_SUCCESS)
        throw std::runtime_error("failed to create window surface!");
}

//-------------------------------------------------------------------------------//

Window::~Window() 
{
    glfwDestroyWindow(window_);
    
    glfwTerminate();
};

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace