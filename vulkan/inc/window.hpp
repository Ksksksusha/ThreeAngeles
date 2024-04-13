#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

//-------------------------------------------------------------------------------//

namespace Vulkan
{

//-------------------------------------------------------------------------------//

class Window
{
    private:

        int width_;

        int height_;

        bool frameBufferResized_ = false;

        std::string window_name_;

        GLFWwindow* window_;

        void InitWindow();

        static void FrameBufferResizeCallback(GLFWwindow* window, int width, int height);

    public:

        Window(int width, int height, const std::string& name);

        ~Window();

        Window(const Window& rhs) = delete;

        Window& operator=(const Window& rhs) = delete;

        bool ShouldClose() { return glfwWindowShouldClose(window_); }

        VkExtent2D GetExtent() { return {static_cast<uint32_t>(width_), static_cast<uint32_t>(height_)}; }

        bool WasWindowResized() { return frameBufferResized_; }

        void ResetWindowResizeFlag() { frameBufferResized_ = false; }

        GLFWwindow* GetWindow() const { return window_; }

        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR *surface);
};

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace

#endif