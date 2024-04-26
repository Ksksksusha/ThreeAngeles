#pragma once

#include "model.hpp"
#include "window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "pipeline.hpp"
#include "descriptor.hpp"
#include "uniform_buffer.hpp"
#include "render.hpp"
#include "camera.hpp"
#include "triangle.hpp"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

namespace Vulkan
{

//-------------------------------------------------------------------------------//

glm::vec3 GetGlmVector(const geometry::point_t& point);

//-------------------------------------------------------------------------------//

glm::vec3 GetNormal(const geometry::triangle_t& triangle);

//-------------------------------------------------------------------------------//

const uint32_t Width  = 800;

const uint32_t Height = 600;

class App 
{
    public:
        
        void run();

        App(std::vector<Model::Vertex> vertices, std::vector<uint32_t> indices) : model(device, vertices, indices) {}

    private:
        
        Camera camera{};
        
        Window window{Width, Height, "vulkan" };
        
        Device device{window };
        
        SwapChain swapChain{window, device };
        
        DescriptorSetLayout descriptorSetLayout{device };
        
        Pipeline pipeline{device, swapChain, descriptorSetLayout, "vert.spv", "frag.spv"};
        
        Model model;
        
        UniformBuffer uniformBuffer{device, swapChain, camera, window };
        
        DescriptorPool descriptorPool{device, swapChain };
        
        DescriptorSets descriptorSets{device, swapChain, uniformBuffer, descriptorSetLayout, descriptorPool };
        
        Render render{device, swapChain, pipeline, model, descriptorSets };
        
        void drawFrame(const float frame_time);
};

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace