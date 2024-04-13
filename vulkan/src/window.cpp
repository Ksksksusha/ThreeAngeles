#include "window.hpp"

#include <stdexcept>

namespace Vulkan
{

//-------------------------------------------------------------------------------//

Window::Window(int width, int height, const std::string& name) : 
    width_{width}, 
    height_{height}, 
    window_name_{name}
{
    InitWindow();
}

//-------------------------------------------------------------------------------//

void Window::InitWindow()
{
    glfwInit();
    
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window_ = glfwCreateWindow(width_, height_, window_name_.c_str(), nullptr, nullptr);

    glfwSetWindowUserPointer(window_, this);

    glfwSetFramebufferSizeCallback(window_, FrameBufferResizeCallback);
}

//-------------------------------------------------------------------------------//

void Window::CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface)
{
    if (glfwCreateWindowSurface(instance, window_, nullptr, surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed create window\n");
    }
}

//-------------------------------------------------------------------------------//

void Window::FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
{
    auto lve_window = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));

    lve_window->frameBufferResized_ = true;

    lve_window->width_              = width;

    lve_window->height_             = height;
}

//-------------------------------------------------------------------------------//

Window::~Window()
{
    glfwDestroyWindow(window_);

    glfwTerminate();
}

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace