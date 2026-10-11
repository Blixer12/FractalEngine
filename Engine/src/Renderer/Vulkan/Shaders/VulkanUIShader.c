#include "VulkanUIShader.h"

#include "Core/Logger.h"
#include "Core/Memory.h"

#include "Renderer/Vulkan/VulkanShaderUtils.h"
#include "Renderer/Vulkan/VulkanPipeline.h"
#include "Renderer/Vulkan/VulkanBuffer.h"

#include "Systems/TextureSystem.h"

#include "Math/MathDef.h"
#include "Math/FMath.h"

constexpr char BuiltinShaderNameUI[] = "Builtin.UIShader";


Bool8 VulkanUIShaderCreate(VulkanContext* Context, VulkanUIShader* Shader) 
{

    // Shader module init per stage.
    char StageTypeStrings[UIShaderStageCount][5] = {"vert", "frag"};
    VkShaderStageFlagBits StageTypes[UIShaderStageCount] = {VK_SHADER_STAGE_VERTEX_BIT, VK_SHADER_STAGE_FRAGMENT_BIT};

    for (UInt32 i = 0; i < UIShaderStageCount; ++i) {
        if (!CreateShaderModule(Context, BuiltinShaderNameUI, StageTypeStrings[i], StageTypes[i], i, Shader->Stages)) {
            FLERROR("Unable to create %s shader module for '%s'.", StageTypeStrings[i], BuiltinShaderNameUI);
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

    Shader->SamplerUses[0] = TEXTURE_USE_MAP_DIFFUSE;

    // Local Descriptors
    VkDescriptorType DescriptorTypes[VulkanUIShaderDescriptorCount] = {
        VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
    };

    VkDescriptorSetLayoutBinding Bindings[VulkanUIShaderDescriptorCount];
    FMZeroMemory(&Bindings, sizeof(VkDescriptorSetLayoutBinding) * VulkanUIShaderDescriptorCount);
    for (UInt32 i = 0; i < VulkanUIShaderDescriptorCount; ++i)
    {
        Bindings[i].binding = i;
        Bindings[i].descriptorCount = 1;
        Bindings[i].descriptorType = DescriptorTypes[i];
        Bindings[i].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    }

    VkDescriptorSetLayoutCreateInfo LayoutInfo = {0};
    LayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    LayoutInfo.bindingCount = 2;
    LayoutInfo.pBindings = Bindings;
    VK_CHECK(vkCreateDescriptorSetLayout(Context->Device.LogicalDevice, &LayoutInfo, Context->Allocator, &Shader->ObjectDescriptorSetLayout));

    VkDescriptorPoolSize ObjectPoolSizes[2];
    ObjectPoolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    ObjectPoolSizes[0].descriptorCount = VulkanMaxUICount;

    ObjectPoolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    ObjectPoolSizes[1].descriptorCount = VulkanUIShaderSamplerCount * VulkanMaxUICount;

    VkDescriptorPoolCreateInfo ObjectPoolInfo = {0};
    ObjectPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    ObjectPoolInfo.poolSizeCount = 2;
    ObjectPoolInfo.pPoolSizes = ObjectPoolSizes;
    ObjectPoolInfo.maxSets = VulkanMaxUICount;
    ObjectPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    VK_CHECK(vkCreateDescriptorPool(Context->Device.LogicalDevice, &ObjectPoolInfo, Context->Allocator, &Shader->ObjectDescriptorPool));

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
    constexpr Int32 AttributeCount = 2;
    VkVertexInputAttributeDescription AttributeDescriptions[AttributeCount];
    
    // Position
    VkFormat Formats[AttributeCount] = {
        VK_FORMAT_R32G32_SFLOAT,
        VK_FORMAT_R32G32_SFLOAT
    };

    UInt64 Sizes[AttributeCount] = {
        sizeof(Vec2),
        sizeof(Vec2)
    };
    for (UInt32 i = 0; i < AttributeCount; ++i)
    {
        AttributeDescriptions[i].binding = 0;
        AttributeDescriptions[i].location = i;
        AttributeDescriptions[i].format = Formats[i];
        AttributeDescriptions[i].offset = Offset;
        Offset += Sizes[i];
    }

    // Descriptor set Layouts
    const Int32 DescriptorSetLayoutCount = 2;
    VkDescriptorSetLayout Layouts[2] = {
        Shader->GlobalDescriptorSetLayout,
        Shader->ObjectDescriptorSetLayout
    };


    VkPipelineShaderStageCreateInfo StageCreateInfos[UIShaderStageCount];
    FMZeroMemory(StageCreateInfos, sizeof(StageCreateInfos));
    for (UInt32 i = 0; i < UIShaderStageCount; ++i)
    {
        StageCreateInfos[i].sType = Shader->Stages[i].ShaderStageCreateInfo.sType;
        StageCreateInfos[i] = Shader->Stages[i].ShaderStageCreateInfo;
    }

    if (!VulkanGraphicsPipelineCreate(
        Context,
        &Context->UIRenderpass,
        sizeof(Vertex2D),
        AttributeCount,
        AttributeDescriptions,
        DescriptorSetLayoutCount,
        Layouts,
        UIShaderStageCount,
        StageCreateInfos,
        Viewport,
        Scissor,
        false,
        false,
        &Shader->Pipeline
    ))
    {
        FLERROR("Failed to load the Graphics Pipeline for the Builtin Object shaders!");
        return false;
    }

    UInt32 DeviceLocalBits = Context->Device.SupportsDeviceLocalHostVisible ? VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT : 0;
    if (!VulkanBufferCreate(
        Context,
        sizeof(VulkanUIShaderGlobalUBO) * 3,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | DeviceLocalBits,
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

    if (!VulkanBufferCreate(
        Context,
        sizeof(VulkanUIShaderInstanceUBO) * VulkanMaxUICount, //* MAX_MATERIAL_INSTANCE_COUNT
        VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        true,
        &Shader->ObjectUniformBuffer))
        {
            FLERROR("Material Instance buffer creation failed for object shader");
            return false;
        }

    return true;
}

void VulkanUIShaderDestroy(VulkanContext* Context, struct VulkanUIShader* Shader) 
{
    VkDevice LogicalDevice = Context->Device.LogicalDevice;

    vkDestroyDescriptorPool(LogicalDevice, Shader->ObjectDescriptorPool, Context->Allocator);
    vkDestroyDescriptorSetLayout(LogicalDevice, Shader->ObjectDescriptorSetLayout, Context->Allocator);


    VulkanBufferDestroy(Context, &Shader->GlobalUniformBuffer);
    VulkanBufferDestroy(Context, &Shader->ObjectUniformBuffer);

    VulkanPipelineDestroy(Context, &Shader->Pipeline);

    vkDestroyDescriptorPool(LogicalDevice, Shader->GlobalDescriptorPool, Context->Allocator);

    vkDestroyDescriptorSetLayout(LogicalDevice, Shader->GlobalDescriptorSetLayout, Context->Allocator);

    // Destroy Shader Modules
    for (UInt32 i = 0; i < UIShaderStageCount; ++i)
    {
        vkDestroyShaderModule(Context->Device.LogicalDevice, Shader->Stages[i].Handle, Context->Allocator);
        Shader->Stages[i].Handle = 0;
    }
}

void VulkanUIShaderUse(VulkanContext* Context, struct VulkanUIShader* Shader) 
{
    UInt32 ImageIndex = Context->ImageIndex;
    VulkanPipelineBind(&Context->GraphicsCommandBuffers[ImageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, Shader->Pipeline);
}

void VulkanUIShaderUpdateGlobalState(VulkanContext* Context, struct VulkanUIShader* Shader, Float32 DeltaTime)
{
    (void)DeltaTime;
    UInt32 ImageIndex = Context->ImageIndex;
    VkCommandBuffer CommandBuffer = Context->GraphicsCommandBuffers[ImageIndex].Handle;
    VkDescriptorSet GlobalDescriptors = Shader->GlobalDescriptorSets[ImageIndex];

    UInt32 Range = sizeof(VulkanUIShaderGlobalUBO);
    UInt64 Offset = sizeof(VulkanUIShaderGlobalUBO) * ImageIndex;

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

void VulkanUIShaderSetModel(VulkanContext* Context, struct VulkanUIShader* Shader, Mat4 Model)
{
    if (Context && Shader)
    {
        UInt32 ImageIndex = Context->ImageIndex;
        VkCommandBuffer CommandBuffer = Context->GraphicsCommandBuffers[ImageIndex].Handle;

        vkCmdPushConstants(CommandBuffer, Shader->Pipeline.PipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(Mat4), &Model);
    }
}

void VulkanUIShaderApplyMaterial(VulkanContext* Context, struct VulkanUIShader* Shader, Material* Material)
{
    if (Context && Shader)
    {
        UInt32 ImageIndex = Context->ImageIndex;
        VkCommandBuffer CommandBuffer = Context->GraphicsCommandBuffers[ImageIndex].Handle;

        VulkanUIShaderInstanceState* InstanceState = &Shader->InstanceStates[Material->InternalID];
        VkDescriptorSet ObjectDescriptorSet = InstanceState->DescriptorSets[ImageIndex];

        // TODO: If needs update
        VkWriteDescriptorSet DescriptorWrites[VulkanUIShaderDescriptorCount];
        FMZeroMemory(DescriptorWrites, sizeof(VkWriteDescriptorSet) * VulkanUIShaderDescriptorCount);
        UInt32 DescriptorCount = 0;
        UInt32 DescriptorIndex = 0;

        UInt32 Range = sizeof(VulkanUIShaderInstanceUBO);
        UInt64 Offset = sizeof(VulkanUIShaderInstanceUBO) * Material->InternalID;
        VulkanUIShaderInstanceUBO InstanceUBO;

        // Base color from Material
        InstanceUBO.BaseColor = Material->BaseColor;

        VulkanBufferLoadData(Context, &Shader->ObjectUniformBuffer, Offset, Range, 0, &InstanceUBO);

        UInt32* GlobalUBOGeneration = &InstanceState->DescriptorStates[DescriptorIndex].Generations[ImageIndex];
        if (*GlobalUBOGeneration == InvalidID || *GlobalUBOGeneration != Material->Generation)
        {
            VkDescriptorBufferInfo BufferInfo;
            BufferInfo.buffer = Shader->ObjectUniformBuffer.Handle;
            BufferInfo.offset = Offset;
            BufferInfo.range = Range;

            VkWriteDescriptorSet Descriptor = {0};
            Descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            Descriptor.dstSet = ObjectDescriptorSet;
            Descriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            Descriptor.descriptorCount = 1;
            Descriptor.pBufferInfo = &BufferInfo;

            DescriptorWrites[DescriptorCount] = Descriptor;
            DescriptorCount++;

            *GlobalUBOGeneration = Material->Generation;
        }

        DescriptorIndex++;

        // Samplers
        const UInt64 SamplerCount = 1;
        VkDescriptorImageInfo ImageInfos[1];
        for (UInt32 SamplerIndex = 0; SamplerIndex < SamplerCount; SamplerIndex++)
        {
            TextureUse Use = Shader->SamplerUses[SamplerIndex];
            Texture* T = 0;
            switch (Use)
            {
                case TEXTURE_USE_MAP_DIFFUSE:
                    T = Material->BaseColorMap.Texture;
                    break;
                default:
                FLERROR("Unable to bind sampler to unknown use");
                return;
            }
            UInt32* DescriptorGeneration = &InstanceState->DescriptorStates[DescriptorIndex].Generations[ImageIndex];
            UInt32* DescriptorID = &InstanceState->DescriptorStates[DescriptorIndex].IDs[ImageIndex];

            // If texture is NOT loaded, use default
            if (T->Generation == InvalidID)
            {
                T = TextureSystemGetDefault();

                *DescriptorGeneration = InvalidID;
            }

            if (T && (*DescriptorGeneration != T->Generation || *DescriptorGeneration == InvalidID || *DescriptorID != T->ID || *DescriptorID == InvalidID))
            {
                VulkanTextureData* InternalData = (VulkanTextureData*)T->InternalData;

                ImageInfos[SamplerIndex].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                ImageInfos[SamplerIndex].imageView = InternalData->Image.View;
                ImageInfos[SamplerIndex].sampler = InternalData->Sampler;

                VkWriteDescriptorSet Descriptor = {0};
                Descriptor.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                Descriptor.dstSet = ObjectDescriptorSet;
                Descriptor.dstBinding = 1;
                Descriptor.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                Descriptor.descriptorCount = 1;
                Descriptor.pImageInfo = &ImageInfos[SamplerIndex];


                DescriptorWrites[DescriptorCount] = Descriptor;
                DescriptorCount++;

                if (T->Generation != InvalidID)
                {
                    *DescriptorGeneration = T->Generation;
                    *DescriptorID = T->ID;
                }

                DescriptorIndex++;
            }
        }

        if (DescriptorCount > 0)
        {
            vkUpdateDescriptorSets(Context->Device.LogicalDevice, DescriptorCount, DescriptorWrites, 0, 0);
        }

        vkCmdBindDescriptorSets(CommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Shader->Pipeline.PipelineLayout, 1, 1, &ObjectDescriptorSet, 0, 0);
    }
}

Bool8 VulkanUIShaderAcquireResources(VulkanContext* Context, struct VulkanUIShader* Shader, Material* Material)
{
    Material->InternalID = Shader->ObjectUniformBufferIndex;
    Shader->ObjectUniformBufferIndex++;

    VulkanUIShaderInstanceState* ObjectState = &Shader->InstanceStates[Material->InternalID];
    for (UInt32 i = 0; i < VulkanUIShaderDescriptorCount; ++i)
    {
        for (UInt32 j = 0; j < 3; j++)
        {
            ObjectState->DescriptorStates[i].Generations[j] = InvalidID;
            ObjectState->DescriptorStates[i].IDs[j] = InvalidID;
        }
        
    }

    VkDescriptorSetLayout Layouts[3] = {
        Shader->ObjectDescriptorSetLayout,
        Shader->ObjectDescriptorSetLayout,
        Shader->ObjectDescriptorSetLayout
    };

    VkDescriptorSetAllocateInfo AllocateInfo = {0};
    AllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    AllocateInfo.descriptorPool = Shader->ObjectDescriptorPool;
    AllocateInfo.descriptorSetCount = 3;
    AllocateInfo.pSetLayouts = Layouts;

    VkResult Result = vkAllocateDescriptorSets(Context->Device.LogicalDevice, &AllocateInfo, ObjectState->DescriptorSets);
    if (Result != VK_SUCCESS)
    {
        FLERROR("Error allocating descriptor sets");
        return false;
    }

    return true;
}

void VulkanUIShaderReleaseResources(VulkanContext* Context, struct VulkanUIShader* Shader, Material* Material)
{
    VulkanUIShaderInstanceState* InstanceState = &Shader->InstanceStates[Material->InternalID];

    const UInt32 DescriptorSetCount = 3;

    vkDeviceWaitIdle(Context->Device.LogicalDevice);

    VkResult Result = vkFreeDescriptorSets(Context->Device.LogicalDevice, Shader->ObjectDescriptorPool, DescriptorSetCount, InstanceState->DescriptorSets);
    if (Result != VK_SUCCESS)
    {
        FLERROR("Error freeing object shader descriptor sets");
    }

    for (UInt32 i = 0; i < VulkanUIShaderDescriptorCount; ++i)
    {
        for (UInt32 j = 0; j < VulkanMaxUICount; j++)
        {
            InstanceState->DescriptorStates[i].Generations[j] = InvalidID;
            InstanceState->DescriptorStates[i].IDs[j] = InvalidID;
        }
        
    }

    Material->InternalID = InvalidID;
    
}