#pragma once

#include "Device.h"

namespace FractalEngine
{

    class FractalBuffer
    {
    public:
        FractalBuffer(
            FractalDevice &device,
            VkDeviceSize instanceSize,
            uint32_t instanceCount,
            VkBufferUsageFlags usageFlags,
            VkMemoryPropertyFlags memoryPropertyFlags,
            VkDeviceSize minOffsetAlignment = 1);
        ~FractalBuffer();

        FractalBuffer(const FractalBuffer &) = delete;
        FractalBuffer &operator=(const FractalBuffer &) = delete;

        VkResult map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        void unmap();

        void writeToBuffer(void *data, VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkDescriptorBufferInfo descriptorInfo(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);
        VkResult invalidate(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

        void writeToIndex(void *data, int index);
        VkResult flushIndex(int index);
        VkDescriptorBufferInfo descriptorInfoForIndex(int index);
        VkResult invalidateIndex(int index);

        VkBuffer getBuffer() const { return Buffer; }
        void *getMappedMemory() const { return Mapped; }
        uint32_t getInstanceCount() const { return InstanceCount; }
        VkDeviceSize getInstanceSize() const { return InstanceSize; }
        VkDeviceSize getAlignmentSize() const { return AlignmentSize; }
        VkBufferUsageFlags getUsageFlags() const { return UsageFlags; }
        VkMemoryPropertyFlags getMemoryPropertyFlags() const { return MemoryPropertyFlags; }
        VkDeviceSize getBufferSize() const { return BufferSize; }

    private:
        static VkDeviceSize getAlignment(VkDeviceSize instanceSize, VkDeviceSize minOffsetAlignment);

        FractalDevice &Device;
        void *Mapped = nullptr;
        VkBuffer Buffer = VK_NULL_HANDLE;
        VkDeviceMemory Memory = VK_NULL_HANDLE;

        VkDeviceSize BufferSize;
        uint32_t InstanceCount;
        VkDeviceSize InstanceSize;
        VkDeviceSize AlignmentSize;
        VkBufferUsageFlags UsageFlags;
        VkMemoryPropertyFlags MemoryPropertyFlags;
    };

} // namespace FractalEngine
