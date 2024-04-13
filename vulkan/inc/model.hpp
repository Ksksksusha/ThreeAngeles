#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "device.hpp"
#include "uniform_buffer.hpp"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

class Model 
{
    public:    
    
        struct Vertex {
            glm::vec3 position;
            glm::vec3 color;
            glm::vec3 normal;

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
        };

        struct Builder {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
        };

        Model(Device &device, const Model::Builder& builder);
        
        ~Model() {};

        Model(const Model &) = delete;
        
        Model &operator=(const Model &) = delete;

        void Bind(VkCommandBuffer commandBuffer);
        
        void Draw(VkCommandBuffer commandBuffer);

    private:
        
        void CreateVertexBuffers(const std::vector<Vertex> &vertices);

        void CreateIndexBuffers(const std::vector<uint32_t> &indices);

        Device          &device_;
        
        std::unique_ptr<UniformBuffer> vertexBuffer_;
        uint32_t        vertexCount_;

        bool hasIndexBuffer = false;

        std::unique_ptr<UniformBuffer> indexBuffer_;
        uint32_t        indexCount_;
};

//-------------------------------------------------------------------------------//

}  // end of Vulkan namespace 

#endif