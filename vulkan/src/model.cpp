#include "model.hpp"
#include "device.hpp"
#include <array>

namespace Vulkan 
{

//-------------------------------------------------------------------------------//

Model::Model(Device& device, std::vector<Model::Vertex> vertices, std::vector<uint32_t> indices) 
    : device_(device) 
{
    vertices_ = std::move(vertices);
    
    indices_  = std::move(indices);

    createVertexBuffer();
    
    createIndexBuffer();
}

//-------------------------------------------------------------------------------//

Model::~Model() 
{
    vkDestroyBuffer(device_.getDevice(), indexBuffer_, nullptr);

    vkFreeMemory(device_.getDevice(), indexBufferMemory_, nullptr);

    vkDestroyBuffer(device_.getDevice(), vertexBuffer_, nullptr);

    vkFreeMemory(device_.getDevice(), vertexBufferMemory_, nullptr);
}

//-------------------------------------------------------------------------------//

void Model::createVertexBuffer()
{
    VkDeviceSize bufferSize = sizeof(vertices_[0]) * vertices_.size();

    VkBuffer stagingBuffer;
    
    VkDeviceMemory stagingBufferMemory;

    device_.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data = nullptr;

    vkMapMemory(device_.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);

    memcpy(data, vertices_.data(), (size_t) bufferSize);
    
    vkUnmapMemory(device_.getDevice(), stagingBufferMemory);
    
    device_.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer_, vertexBufferMemory_);

    device_.copyBuffer(stagingBuffer, vertexBuffer_, bufferSize);

    vkDestroyBuffer(device_.getDevice(), stagingBuffer, nullptr);

    vkFreeMemory(device_.getDevice(), stagingBufferMemory, nullptr);
}

//-------------------------------------------------------------------------------//

void Model::createIndexBuffer() 
{
    VkDeviceSize bufferSize = sizeof(indices_[0]) * indices_.size();

    VkBuffer stagingBuffer;
    
    VkDeviceMemory stagingBufferMemory;
    
    device_.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data = nullptr;

    vkMapMemory(device_.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);

    memcpy(data, indices_.data(), (size_t) bufferSize);

    vkUnmapMemory(device_.getDevice(), stagingBufferMemory);
    
    device_.createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer_, indexBufferMemory_);

    device_.copyBuffer(stagingBuffer, indexBuffer_, bufferSize);

    vkDestroyBuffer(device_.getDevice(), stagingBuffer, nullptr);

    vkFreeMemory(device_.getDevice(), stagingBufferMemory, nullptr);
}

//-------------------------------------------------------------------------------//

VkVertexInputBindingDescription Model::Vertex::getBindingDescription() 
{
    VkVertexInputBindingDescription bindingDescription{};
    
    bindingDescription.binding = 0;
    
    bindingDescription.stride = sizeof(Vertex);
    
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

//-------------------------------------------------------------------------------//

std::array<VkVertexInputAttributeDescription, 3> Model::Vertex::getAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions{};
    
    attributeDescriptions[0].binding = 0;
    
    attributeDescriptions[0].location = 0;
    
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    
    attributeDescriptions[0].offset = offsetof(Vertex, pos);

    attributeDescriptions[1].binding = 0;
    
    attributeDescriptions[1].location = 1;
    
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    
    attributeDescriptions[1].offset = offsetof(Vertex, color);

    attributeDescriptions[2].binding = 0;
    
    attributeDescriptions[2].location = 2;
    
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    
    attributeDescriptions[2].offset = offsetof(Vertex, normal);

    return attributeDescriptions;
}

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace