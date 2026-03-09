#pragma once

#include "Device.h"

// Librarys
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

// Standard Library
#include <vector>

namespace FractalEngine
{
    class FractalModel
    {

    public:
        struct Vertex
        {
            glm::vec2 Position;
            glm::vec4 Color;

            static std::vector<VkVertexInputBindingDescription> GetBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();
        };

        FractalModel(FractalDevice &Device, const std::vector<Vertex> &Vertices);
        ~FractalModel();

        FractalModel(const FractalModel &) = delete;
        FractalModel &operator=(const FractalModel &) = delete;

        void Bind(VkCommandBuffer CommandBuffer);
        void Draw(VkCommandBuffer CommandBuffer);

    private:
        void CreateVertexBuffers(const std::vector<Vertex> &Vertices);

        FractalDevice &Device;
        VkBuffer VertexBuffer;
        VkDeviceMemory VertexBufferMemory;
        uint32_t VertexCount;
    };
}