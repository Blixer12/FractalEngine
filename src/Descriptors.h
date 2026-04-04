#pragma once

#include "Device.h"

// std
#include <memory>
#include <unordered_map>
#include <vector>

namespace FractalEngine
{

    class FractalDescriptorSetLayout
    {
    public:
        class Builder
        {
        public:
            Builder(FractalDevice &fractalDevice) : fractalDevice{fractalDevice} {}

            Builder &addBinding(
                uint32_t binding,
                VkDescriptorType descriptorType,
                VkShaderStageFlags stageFlags,
                uint32_t count = 1);
            std::unique_ptr<FractalDescriptorSetLayout> build() const;

        private:
            FractalDevice &fractalDevice;
            std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings{};
        };

        FractalDescriptorSetLayout(
            FractalDevice &fractalDevice, std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings);
        ~FractalDescriptorSetLayout();
        FractalDescriptorSetLayout(const FractalDescriptorSetLayout &) = delete;
        FractalDescriptorSetLayout &operator=(const FractalDescriptorSetLayout &) = delete;

        VkDescriptorSetLayout getDescriptorSetLayout() const { return descriptorSetLayout; }

    private:
        FractalDevice &fractalDevice;
        VkDescriptorSetLayout descriptorSetLayout;
        std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding> bindings;

        friend class FractalDescriptorWriter;
    };

    class FractalDescriptorPool
    {
    public:
        class Builder
        {
        public:
            Builder(FractalDevice &fractalDevice) : fractalDevice{fractalDevice} {}

            Builder &addPoolSize(VkDescriptorType descriptorType, uint32_t count);
            Builder &setPoolFlags(VkDescriptorPoolCreateFlags flags);
            Builder &setMaxSets(uint32_t count);
            std::unique_ptr<FractalDescriptorPool> build() const;

        private:
            FractalDevice &fractalDevice;
            std::vector<VkDescriptorPoolSize> poolSizes{};
            uint32_t maxSets = 1000;
            VkDescriptorPoolCreateFlags poolFlags = 0;
        };

        FractalDescriptorPool(
            FractalDevice &fractalDevice,
            uint32_t maxSets,
            VkDescriptorPoolCreateFlags poolFlags,
            const std::vector<VkDescriptorPoolSize> &poolSizes);
        ~FractalDescriptorPool();
        FractalDescriptorPool(const FractalDescriptorPool &) = delete;
        FractalDescriptorPool &operator=(const FractalDescriptorPool &) = delete;

        bool allocateDescriptor(
            const VkDescriptorSetLayout descriptorSetLayout, VkDescriptorSet &descriptor) const;

        void freeDescriptors(std::vector<VkDescriptorSet> &descriptors) const;

        void resetPool();

    private:
        FractalDevice &fractalDevice;
        VkDescriptorPool descriptorPool;

        friend class FractalDescriptorWriter;
    };

    class FractalDescriptorWriter
    {
    public:
        FractalDescriptorWriter(FractalDescriptorSetLayout &setLayout, FractalDescriptorPool &pool);

        FractalDescriptorWriter &writeBuffer(uint32_t binding, VkDescriptorBufferInfo *bufferInfo);
        FractalDescriptorWriter &writeImage(uint32_t binding, VkDescriptorImageInfo *imageInfo);

        bool build(VkDescriptorSet &set);
        void overwrite(VkDescriptorSet &set);

    private:
        FractalDescriptorSetLayout &setLayout;
        FractalDescriptorPool &pool;
        std::vector<VkWriteDescriptorSet> writes;
    };

} // namespace FractalEngine