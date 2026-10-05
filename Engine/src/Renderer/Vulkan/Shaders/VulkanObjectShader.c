#include "VulkanObjectShader.h"

#include "Core/Logger.h"
#include "Core/Memory.h"

#include "Renderer/Vulkan/VulkanShaderUtils.h"
#include "Renderer/Vulkan/VulkanPipeline.h"
#include "Renderer/Vulkan/VulkanBuffer.h"

#include "Math/MathDef.h"

#define BUILTIN_SHADER_NAME_OBJECT "Builtin.ObjectShader"


Bool8 VulkanObjectShaderCreate(VulkanContext* Context, VulkanObjectShader* Shader) {
    // Shader module init per stage.
    char StageTypeStrings[OBJECT_SHADER_STAGE_COUNT][5] = {"vert", "frag"};
    VkShaderStageFlagBits StageTypes[OBJECT_SHADER_STAGE_COUNT] = {VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};

    for (UInt32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; i++) {
        if (!CreateShaderModule(Context, BUILTIN_SHADER_NAME_OBJECT, StageTypeStrings[i], StageTypes[i], i, Shader->Stages)) {
            FLERROR("Unable to create %s shader module for '%s'.", StageTypeStrings[i], BUILTIN_SHADER_NAME_OBJECT);
            return false;
        }
    }

    // Global Descriptors
    VkDescriptorSetLayoutBinding GlobalUBOBinding;
    GlobalUBOBinding.binding = 0;
    GlobalUBOBinding.descriptorCount = 1;
    GlobalUBOBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    GlobalUBOBinding.pImmutableSamplers = 0;
    GlobalUBOBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

    VkDescriptorSetLayoutCreateInfo GlobalLayoutInfo = {0};
    GlobalLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    GlobalLayoutInfo.bindingCount = 1;
    GlobalLayoutInfo.pBindings = &GlobalUBOBinding;
    VK_CHECK(vkCreateDescriptorSetLayout(Context->Device.LogicalDevice, &GlobalLayoutInfo, Context->Allocator, &Shader->GlobalDescriptorSetLayout));
    
    VkDescriptorPoolSize GlobalPoolSize;
    GlobalPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    GlobalPoolSize.descriptorCount = Context->Swapchain.ImageCount;

    VkDescriptorPoolCreateInfo GlobalPoolInfo = {0};
    GlobalPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    GlobalPoolInfo.poolSizeCount = 1;
    GlobalPoolInfo.pPoolSizes = &GlobalPoolSize;
    GlobalPoolInfo.maxSets = Context->Swapchain.ImageCount;

    VK_CHECK(vkCreateDescriptorPool(Context->Device.LogicalDevice, &GlobalPoolInfo, Context->Allocator, &Shader->GlobalDescriptorPool));

    // Pipeline Generation
    VkViewport Viewport;
    Viewport.x = 0.0f;
    Viewport.y = (Float32)Context->FramebufferHeight;
    Viewport.width = (Float32)Context->FramebufferWidth;
    Viewport.height = -(Float32)Context->FramebufferHeight;
    Viewport.minDepth = 0.0f;
    Viewport.maxDepth = 1.0f;

    VkRect2D Scissor;
    Scissor.offset.x = Scissor.offset.y = 0;
    Scissor.extent.width = Context->FramebufferWidth;
    Scissor.extent.height = Context->FramebufferHeight;

    // Attributes
    UInt32 Offset = 0;
    constexpr Int32 AttributeCount = 1;
    VkVertexInputAttributeDescription AttributeDescriptions[AttributeCount];
    
    // Position
    VkFormat Formats[AttributeCount] = {
        VK_FORMAT_R32G32B32_SFLOAT
    };

    UInt64 Sizes[AttributeCount] = {
        sizeof(Vec3)
    };
    for (UInt32 i = 0; i < AttributeCount; i++)
    {
        AttributeDescriptions[i].binding = 0;
        AttributeDescriptions[i].location = i;
        AttributeDescriptions[i].format = Formats[i];
        AttributeDescriptions[i].offset = Offset;
        Offset += Sizes[i];
    }

    // Descriptor set Layouts
    const Int32 DescriptorSetLayoutCount = 1;
    VkDescriptorSetLayout Layouts[1] = {
        Shader->GlobalDescriptorSetLayout
    };


    VkPipelineShaderStageCreateInfo StageCreateInfos[OBJECT_SHADER_STAGE_COUNT];
    FMZeroMemory(StageCreateInfos, sizeof(StageCreateInfos));
    for (UInt32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; i++)
    {
        StageCreateInfos[i].sType = Shader->Stages[i].ShaderStageCreateInfo.sType;
        StageCreateInfos[i] = Shader->Stages[i].ShaderStageCreateInfo;
    }

    if (!VulkanGraphicsPipelineCreate(
        Context,
        &Context->MainRenderpass,
        AttributeCount,
        AttributeDescriptions,
        DescriptorSetLayoutCount,
        Layouts,
        OBJECT_SHADER_STAGE_COUNT,
        StageCreateInfos,
        Viewport,
        Scissor,
        false,
        &Shader->Pipeline
    ))
    {
        FLERROR("Failed to load the Graphics Pipeline for the Builtin Object shaders!");
        return false;
    }

    if (!VulkanBufferCreate(
        Context,
        sizeof(GlobalUniformObject) * 3,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        true,
        &Shader->GlobalUniformBuffer))
        {
            FLERROR("Vulkan buffer creation failed for object shader");
            return false;
        }

    VkDescriptorSetLayout GlobalLayouts[3] = {
        Shader->GlobalDescriptorSetLayout,
        Shader->GlobalDescriptorSetLayout,
        Shader->GlobalDescriptorSetLayout
    };

    VkDescriptorSetAllocateInfo AllocateInfo = {0};
    AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    AllocateInfo.descriptorPool = Shader->GlobalDescriptorPool;
    AllocateInfo.descriptorSetCount = 3;
    AllocateInfo.pSetLayouts = GlobalLayouts;

    VK_CHECK(vkAllocateDescriptorSets(Context->Device.LogicalDevice, &AllocateInfo, Shader->GlobalDescriptorSets));

    return true;
}

void VulkanObjectShaderDestroy(VulkanContext* Context, struct VulkanObjectShader* Shader) 
{
    VkDevice LogicalDevice = Context->Device.LogicalDevice;

    VulkanBufferDestroy(Context, &Shader->GlobalUniformBuffer);

    VulkanPipelineDestroy(Context, &Shader->Pipeline);

    vkDestroyDescriptorPool(LogicalDevice, Shader->GlobalDescriptorPool, Context->Allocator);

    vkDestroyDescriptorSetLayout(LogicalDevice, Shader->GlobalDescriptorSetLayout, Context->Allocator);

    // Destroy Shader Modules
    for (UInt32 i = 0; i < OBJECT_SHADER_STAGE_COUNT; i++)
    {
        vkDestroyShaderModule(Context->Device.LogicalDevice, Shader->Stages[i].Handle, Context->Allocator);
        Shader->Stages[i].Handle = 0;
    }
}

void VulkanObjectShaderUse(VulkanContext* Context, struct VulkanObjectShader* Shader) 
{
    UInt32 ImageIndex = Context->ImageIndex;
    VulkanPipelineBind(&Context->GraphicsCommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, Shader->Pipeline);
}

void VulkanObjectShaderUpdateGlobalState(VulkanContext* Context, struct VulkanObjectShader* Shader)
{
    UInt32 ImageIndex = Context->ImageIndex;
    VkCommandBuffer CommandBuffer = Context->GraphicsCommandBuffers[ImageIndex].Handle;
    VkDescriptorSet GlobalDescriptors = Shader->GlobalDescriptorSets[ImageIndex];

    UInt32 Range = sizeof(GlobalUniformObject);
    UInt64 Offset = sizeof(GlobalUniformObject) * ImageIndex;

    VulkanBufferLoadData(Context, &Shader->GlobalUniformBuffer, Offset, Range, 0, &Shader->GlobalUBO);

    VkDescriptorBufferInfo BufferInfo;
    BufferInfo.buffer = Shader->GlobalUniformBuffer.Handle;
    BufferInfo.offset = Offset;
    BufferInfo.range = Range;

    VkWriteDescriptorSet DescriptorWrite = {0};
    DescriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    DescriptorWrite.dstSet = Shader->GlobalDescriptorSets[ImageIndex];
    DescriptorWrite.dstBinding = 0;
    DescriptorWrite.dstArrayElement = 0;
    DescriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    DescriptorWrite.descriptorCount = 1;
    DescriptorWrite.pBufferInfo = &BufferInfo;

    vkUpdateDescriptorSets(Context->Device.LogicalDevice, 1, &DescriptorWrite, 0, 0);
        
    vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Shader->Pipeline.PipelineLayout, 0, 1, &GlobalDescriptors, 0, 0);
}

void VulkanObjectShaderUpdateObject(VulkanContext* Context, struct VulkanObjectShader* Shader, Mat4 Model)
{
    UInt32 ImageIndex = Context->ImageIndex;
    VkCommandBuffer CommandBuffer = Context->GraphicsCommandBuffers[ImageIndex].Handle;

    vkCmdPushConstants(CommandBuffer, Shader->Pipeline.PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Mat4), &Model);
}