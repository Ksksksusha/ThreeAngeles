#include "model.hpp"

#include <cassert>
#include <cstring>

namespace Vulkan
{

//-------------------------------------------------------------------------------//

Model::Model(Device &device, const Model::Builder& builder) : device_{device}
{
    CreateVertexBuffers(builder.vertices);

    CreateIndexBuffers(builder.indices);
}

//-------------------------------------------------------------------------------//

void Model::CreateVertexBuffers(const std::vector<Vertex>& vertices)
{
    vertexCount_ = static_cast<uint32_t>(vertices.size());

    assert(vertexCount_ >= 3 && "Vertex count must be at least 3");

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount_;

    uint32_t vertexSize = sizeof(vertices[0]);

    UniformBuffer stagingBuffer {
        device_, 
        vertexSize, 
        vertexCount_, 
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    stagingBuffer.map();

    stagingBuffer.writeToBuffer((void *)vertices.data());

    vertexBuffer_ = std::make_unique<UniformBuffer>(
        device_,
        vertexSize,
        vertexCount_,
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    device_.copyBuffer(stagingBuffer.getBuffer(), vertexBuffer_->getBuffer(), bufferSize);
}

//-------------------------------------------------------------------------------//

void Model::CreateIndexBuffers(const std::vector<uint32_t>& indices)
{
    indexCount_ = static_cast<uint32_t>(indices.size());
    
    hasIndexBuffer = indexCount_ > 0;

    if (!hasIndexBuffer)
        return;

    VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount_;
    
    uint32_t indexSize = sizeof(indices[0]);

    UniformBuffer stagingBuffer{
        device_,
        indexSize,
        indexCount_,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
    };

    stagingBuffer.map();
    stagingBuffer.writeToBuffer((void *)indices.data());

    indexBuffer_ = std::make_unique<UniformBuffer>(
        device_,
        indexSize,
        indexCount_,
        VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    device_.copyBuffer(stagingBuffer.getBuffer(), indexBuffer_->getBuffer(), bufferSize);
}

//-------------------------------------------------------------------------------//

void Model::Bind(VkCommandBuffer commandBuffer)
{
    VkBuffer buffers[] = {vertexBuffer_->getBuffer()};

    VkDeviceSize offsets[] = {0};

    vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

    if (hasIndexBuffer)
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer_->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
}

//-------------------------------------------------------------------------------//
    
void Model::Draw(VkCommandBuffer commandBuffer)
{
    if (hasIndexBuffer)
        vkCmdDrawIndexed(commandBuffer, indexCount_, 1, 0, 0, 0);

    else
        vkCmdDraw(commandBuffer, vertexCount_, 1, 0, 0);
}

//-------------------------------------------------------------------------------//

std::vector<VkVertexInputBindingDescription> Model::Vertex::GetBindingDescriptions() 
{
    std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
    
    bindingDescriptions[0].binding = 0;
    
    bindingDescriptions[0].stride = sizeof(Vertex);
    
    bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    
    return bindingDescriptions;
}

//-------------------------------------------------------------------------------//

std::vector<VkVertexInputAttributeDescription> Model::Vertex::GetAttributeDescriptions() 
{
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions(3);
    
    
    attributeDescriptions[0].binding    = 0;
    
    attributeDescriptions[0].location   = 0;
    
    attributeDescriptions[0].format     = VK_FORMAT_R32G32B32_SFLOAT;
    
    attributeDescriptions[0].offset     = offsetof(Vertex, position);

    
    attributeDescriptions[1].binding    = 0;
    
    attributeDescriptions[1].location   = 1;
    
    attributeDescriptions[1].format     = VK_FORMAT_R32G32B32_SFLOAT;
    
    attributeDescriptions[1].offset     = offsetof(Vertex, color);

    
    attributeDescriptions[2].binding    = 0;
    
    attributeDescriptions[2].location   = 2;
    
    attributeDescriptions[2].format     = VK_FORMAT_R32G32B32_SFLOAT;
    
    attributeDescriptions[2].offset     = offsetof(Vertex, normal);
    
    
    return attributeDescriptions;
}

//-------------------------------------------------------------------------------//

} // end of Vulkan namespace

//-------------------------------------------------------------------------------//