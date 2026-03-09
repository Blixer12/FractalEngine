#include "Model.h"

// Standard Library
#include <cassert>
#include <cstring>

namespace FractalEngine
{
    FractalModel::FractalModel(FractalDevice &Device, const std::vector<Vertex> &Vertices) : Device{Device}
    {
        CreateVertexBuffers(Vertices);
    }
    FractalModel::~FractalModel()
    {
        vkDestroyBuffer(Device.device(), VertexBuffer, nullptr);
        vkFreeMemory(Device.device(), VertexBufferMemory, nullptr);
    }

    void FractalModel::CreateVertexBuffers(const std::vector<Vertex> &Vertices)
    {
        VertexCount = static_cast<uint32_t>(Vertices.size());
        assert(VertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize BufferSize = sizeof(Vertices[0]) * VertexCount;

        Device.createBuffer(
            BufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            VertexBuffer,
            VertexBufferMemory);

        void *Data;

        vkMapMemory(Device.device(), VertexBufferMemory, 0, BufferSize, 0, &Data);
        memcpy(Data, Vertices.data(), static_cast<size_t>(BufferSize));
        vkUnmapMemory(Device.device(), VertexBufferMemory);
    }

    void FractalModel::Draw(VkCommandBuffer CommandBuffer)
    {
        vkCmdDraw(CommandBuffer, VertexCount, 1, 0, 0);
    }

    void FractalModel::Bind(VkCommandBuffer CommandBuffer)
    {
        VkBuffer Buffers[] = {VertexBuffer};
        VkDeviceSize Offsets[] = {0};

        vkCmdBindVertexBuffers(CommandBuffer, 0, 1, Buffers, Offsets);
    }

    std::vector<VkVertexInputBindingDescription> FractalModel::Vertex::GetBindingDescriptions()
    {
        std::vector<VkVertexInputBindingDescription> BindingDescriptions(1);
        BindingDescriptions[0].binding = 0;
        BindingDescriptions[0].stride = sizeof(Vertex);
        BindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return BindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> FractalModel::Vertex::GetAttributeDescriptions()
    {
        std::vector<VkVertexInputAttributeDescription> AttributeDescription(2);
        AttributeDescription[0].binding = 0;
        AttributeDescription[0].location = 0;
        AttributeDescription[0].format = VK_FORMAT_R32G32_SFLOAT;
        AttributeDescription[0].offset = offsetof(Vertex, Position);

        AttributeDescription[1].binding = 0;
        AttributeDescription[1].location = 1;
        AttributeDescription[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        AttributeDescription[1].offset = offsetof(Vertex, Color);

        return AttributeDescription;
    }
}