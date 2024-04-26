#ifndef WINDOW_HPP_
#define WINDOW_HPP_

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

class Window 
{
    public:
    
        Window(uint32_t width, uint32_t height, std::string windowName);

        ~Window();

        Window(const Window&) = delete;
        
        Window& operator=(const Window &) = delete;

        void CreateWindowSurface(VkInstance instance, VkSurfaceKHR * surface);

        uint32_t GetWidth()             const { return width_; }
        
        uint32_t GetHeight()            const { return height_; }
        
        bool WasFrameBufferResized()    const { return frameBufferResized_; }

        void ResetFrameBufferRisizedFlag() { frameBufferResized_ = false; }
        
        void SetFrameBufferRisizedFlag()   { frameBufferResized_ = true; }

        GLFWwindow* GetGLFWwindow()           { return window_; }
        
        bool ShouldClose()                    { return glfwWindowShouldClose(window_); }


    private:
        
        uint32_t width_, height_;
        
        std::string windowName_;
        
        GLFWwindow* window_;

        bool frameBufferResized_ = false;

        static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
        
        void InitWindow();
};

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace

#endif