#ifndef MODEL_HPP_
#define MODEL_HPP_

#include "device.hpp"
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


namespace Vulkan 
{
    
//-------------------------------------------------------------------------------//

class Model 
{
    public:

        struct Vertex {
            glm::vec3 pos;
            glm::vec3 color;
            glm::vec3 normal;

            static VkVertexInputBindingDescription                  getBindingDescription();
            static std::array<VkVertexInputAttributeDescription, 3> getAttributeDescriptions();
        };
    
        Model(Device& device, std::vector<Model::Vertex> vertices, std::vector<uint32_t> indices);
        
        ~Model();

        Model(const Model&) = delete;
        
        Model& operator=(const Model&) = delete;

        std::vector<Vertex> vertices_;
        
        std::vector<uint32_t> indices_;

        const VkBuffer& getVertexBuffer() const { return vertexBuffer_; }
        
        const VkBuffer& getIndexBuffer()  const { return indexBuffer_;  }
    
    private:
        
        VkBuffer        vertexBuffer_;
        
        VkDeviceMemory  vertexBufferMemory_;
        
        VkBuffer        indexBuffer_;
        
        VkDeviceMemory  indexBufferMemory_;
        
        Device&         device_;

        void createVertexBuffer();
        
        void createIndexBuffer();
};

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace

#endif