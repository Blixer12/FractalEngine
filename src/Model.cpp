#include "Model.h"
#include "Utils.h"

// Libraries
#define TINYOBJLOADER_IMPLEMENTATION
#include <TinyObjectLoader/ObjectLoader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

// Standard Library
#include <cassert>
#include <cstring>
#include <unordered_map>
#include <iostream>

namespace std
{
    template <>
    struct hash<FractalEngine::FractalModel::Vertex>
    {
        size_t operator()(FractalEngine::FractalModel::Vertex const &Vertex) const
        {
            size_t Seed = 0;
            FractalEngine::HashCombine(Seed, Vertex.Position, Vertex.Normal, Vertex.UV, Vertex.Color);
            return Seed;
        }
    };
}

namespace FractalEngine
{
    FractalModel::FractalModel(FractalDevice &Device, const FractalModel::Data &Data) : Device{Device}
    {
        CreateVertexBuffers(Data.Vertices);
        CreateIndexBuffers(Data.Indices);
    }
    FractalModel::~FractalModel()
    {
    }

    std::unique_ptr<FractalModel> FractalModel::CreateModelFromFile(FractalDevice &Device, const std::string &FilePath)
    {
        Data ModelData{};
        ModelData.LoadModel(FilePath);
        return std::make_unique<FractalModel>(Device, ModelData);
    }

    void FractalModel::CreateVertexBuffers(const std::vector<Vertex> &Vertices)
    {
        VertexCount = static_cast<uint32_t>(Vertices.size());
        assert(VertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize BufferSize = sizeof(Vertices[0]) * VertexCount;
        uint32_t VertexSize = sizeof(Vertices[0]);

        FractalBuffer StagingBuffer{
            Device,
            VertexSize,
            VertexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

        StagingBuffer.map();
        StagingBuffer.writeToBuffer((void *)Vertices.data());

        VertexBuffer = std::make_unique<FractalBuffer>(
            Device,
            VertexSize,
            VertexCount,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        Device.copyBuffer(StagingBuffer.getBuffer(), VertexBuffer->getBuffer(), BufferSize);
    }

    void FractalModel::CreateIndexBuffers(const std::vector<uint32_t> &Indices)
    {
        IndexCount = static_cast<uint32_t>(Indices.size());
        HasIndexBuffer = IndexCount > 0;

        if (!HasIndexBuffer)
        {
            return;
        }

        VkDeviceSize BufferSize = sizeof(Indices[0]) * IndexCount;
        uint32_t IndexSize = sizeof(Indices[0]);

        FractalBuffer StagingBuffer{
            Device,
            IndexSize,
            IndexCount,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT};

        StagingBuffer.map();
        StagingBuffer.writeToBuffer((void *)Indices.data());

        IndexBuffer = std::make_unique<FractalBuffer>(
            Device,
            IndexSize,
            IndexCount,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

        Device.copyBuffer(StagingBuffer.getBuffer(), IndexBuffer->getBuffer(), BufferSize);
    }

    void FractalModel::Draw(VkCommandBuffer CommandBuffer)
    {
        if (HasIndexBuffer)
        {
            vkCmdDrawIndexed(CommandBuffer, IndexCount, 1, 0, 0, 0);
        }
        else
        {
            vkCmdDraw(CommandBuffer, VertexCount, 1, 0, 0);
        }
    }

    void FractalModel::Bind(VkCommandBuffer CommandBuffer)
    {
        VkBuffer Buffers[] = {VertexBuffer->getBuffer()};
        VkDeviceSize Offsets[] = {0};

        vkCmdBindVertexBuffers(CommandBuffer, 0, 1, Buffers, Offsets);

        if (HasIndexBuffer)
        {
            vkCmdBindIndexBuffer(CommandBuffer, IndexBuffer->getBuffer(), 0, VK_INDEX_TYPE_UINT32);
        }
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
        std::vector<VkVertexInputAttributeDescription> AttributeDescriptions{};

        AttributeDescriptions.push_back({0, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Position)}); // Position
        AttributeDescriptions.push_back({1, 0, VK_FORMAT_R32G32B32_SFLOAT, offsetof(Vertex, Normal)});   // Normal
        AttributeDescriptions.push_back({2, 0, VK_FORMAT_R32G32_SFLOAT, offsetof(Vertex, UV)});          // UV
        AttributeDescriptions.push_back({3, 0, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Vertex, Color)}); // Color

        return AttributeDescriptions;
    }

    void FractalModel::Data::LoadModel(const std::string &FilePath)
    {
        tinyobj::attrib_t Attributes;
        std::vector<tinyobj::shape_t> Shapes;
        std::vector<tinyobj::material_t> Materials;
        std::string Warn, Error;

        if (!tinyobj::LoadObj(&Attributes, &Shapes, &Materials, &Warn, &Error, FilePath.c_str(), nullptr, true))
        {
            throw std::runtime_error(Warn + Error);
        }

        Vertices.clear();
        Indices.clear();

        std::unordered_map<Vertex, uint32_t> UniqueVertices{};

        for (const auto &Shape : Shapes)
        {
            for (const auto &Index : Shape.mesh.indices)
            {
                Vertex Vertex{};

                if (Index.vertex_index >= 0)
                {
                    Vertex.Position = {
                        Attributes.vertices[3 * Index.vertex_index + 0],
                        Attributes.vertices[3 * Index.vertex_index + 1],
                        Attributes.vertices[3 * Index.vertex_index + 2]};

                    if (!Attributes.colors.empty())
                    {
                        Vertex.Color = {
                            Attributes.colors[3 * Index.vertex_index + 0],
                            Attributes.colors[3 * Index.vertex_index + 1],
                            Attributes.colors[3 * Index.vertex_index + 2],
                            1.0f};
                    }
                    else
                    {
                        Vertex.Color = {1.f, 1.f, 1.f, 1.f};
                    }
                }

                Vertex.Normal = {0.0f, 0.0f, 1.0f}; // default normal if not provided

                if (Index.normal_index >= 0)
                {
                    Vertex.Normal = {
                        Attributes.normals[3 * Index.normal_index + 0],
                        Attributes.normals[3 * Index.normal_index + 1],
                        Attributes.normals[3 * Index.normal_index + 2]};
                }

                Vertex.UV = {0.0f, 0.0f}; // default UV if not provided

                if (Index.texcoord_index >= 0)
                {
                    Vertex.UV = {
                        Attributes.texcoords[2 * Index.texcoord_index + 0],
                        1.0f - Attributes.texcoords[2 * Index.texcoord_index + 1]};
                }

                if (UniqueVertices.count(Vertex) == 0)
                {
                    UniqueVertices[Vertex] = static_cast<uint32_t>(Vertices.size());
                    Vertices.push_back(Vertex);
                }

                Indices.push_back(UniqueVertices[Vertex]);
            }
        }
    }
}