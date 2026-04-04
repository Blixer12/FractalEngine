#pragma once

#include "Buffer.h"
#include "Device.h"

// Librarys
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// Standard Library
#include <memory>
#include <vector>

namespace FractalEngine
{
    class FractalModel
    {

    public:
        struct Vertex
        {
            glm::vec3 Position;
            glm::vec3 Normal;
            glm::vec2 UV;
            glm::vec4 Color;

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

            bool operator==(const Vertex &Other) const
            {
                return Position == Other.Position && Normal == Other.Normal && UV == Other.UV && Color == Other.Color;
            }
        };

        struct Data
        {
            std::vector<Vertex> Vertices{};
            std::vector<uint32_t> Indices{};

            void LoadModel(const std::string &FilePath);
        };

        FractalModel(FractalDevice &Device, const FractalModel::Data &Data);
        ~FractalModel();

        FractalModel(const FractalModel &) = delete;
        FractalModel &operator=(const FractalModel &) = delete;

        static std::unique_ptr<FractalModel> CreateModelFromFile(FractalDevice &Device, const std::string &FilePath);

        void Bind(VkCommandBuffer CommandBuffer);
        void Draw(VkCommandBuffer CommandBuffer);

    private:
        void CreateVertexBuffers(const std::vector<Vertex> &Vertices);
        void CreateIndexBuffers(const std::vector<uint32_t> &Indices);

        FractalDevice &Device;

        std::unique_ptr<FractalBuffer> VertexBuffer;
        uint32_t VertexCount;

        bool HasIndexBuffer = false;

        std::unique_ptr<FractalBuffer> IndexBuffer;
        uint32_t IndexCount;
    };
}