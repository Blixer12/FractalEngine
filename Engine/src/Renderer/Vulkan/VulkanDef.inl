/**
 * @file VulkanDef.inl
 * @author Blixer12
 * @brief This file contains a collection of Vulkan-specific types used
 * for the Vulkan backend.
 * @version 1.0
 * @date 2026-10-11
 * 
 * @copyright Fractal Engine is Copyright (c) Blixer12 2026
 * 
 */

#pragma once

#include <vulkan/vulkan.h>
#include "Renderer/CrystalDef.inl"

#include "Defines.h"

#include "Core/Logger.h"
#include "Core/Asserts.h"

#define Gigabytes(value) ((value) * 1024ULL * 1024ULL * 1024ULL - 1)
#define Megabytes(value) ((value) * 1024ULL * 1024ULL)

/**
 * @brief Checks the given expression's return value against VK_SUCCESS.
 * @param expr The expression whose result should be checked.
 */
#define VK_CHECK(expr)                                                                                          \
do {                                                                                                            \
        VkResult _Result = (expr);                                                                              \
        if ((_Result) < VK_SUCCESS) {                                                                           \
            FLERROR("Vulkan Error %i executing: %s (File: %s, Line: %d)", _Result, #expr, __FILE__, __LINE__);  \
            FASSERT_MSG(false, "Vulkan runtime crash!");                                                        \
        }                                                                                                       \
        else if (_Result == VK_INCOMPLETE ||                                                                    \
                 _Result == VK_PIPELINE_BINARY_MISSING_KHR ||                                                   \
                 _Result == VK_INCOMPATIBLE_SHADER_BINARY_EXT) {                                                \
            FLDEBUG("Vulkan Cache/Query Status %i executing: %s", _Result, #expr);                              \
        }                                                                                                       \
        else if (_Result != VK_SUCCESS && _Result != VK_NOT_READY && _Result != VK_TIMEOUT) {                   \
            FLDEBUG("Vulkan Status Flag %i executing: %s", _Result, #expr);                                     \
        }                                                                                                       \
   } while(0)                                                                                                   \

typedef struct VulkanPhysicalDevicePreferences {
    // --- DEVICE TYPE & MEMORY ---
    Bool8 DiscreteGPU;
    UInt64 MinimumPreferredVRAM;

    // --- MODERN PIPELINE EXTENSIONS ---      
    Bool8 ShaderObjects;          // (VK_EXT_shader_object)
    Bool8 GraphicsPipelineLibrary;// (VK_EXT_graphics_pipeline_library)
    Bool8 DescriptorIndexing;

    // --- NEXT-GEN GEOMETRY & COMPUTE ---
    Bool8 RayTracing;             // Hardware acceleration structures & pipelines
    Bool8 MeshShaders;            // Task/Mesh shader support (bypasses vertex stage)
    Bool8 CooperativeMatrix;      // AI/Matrix core hardware access

    // --- HARDWARE FEATURES ---
    Bool8 GeometryShader;
    Bool8 WireframeMode;          // fillModeNonSolid
    Bool8 TimelineSemaphores;     // Modern CPU/GPU synchronization
    Bool8 BufferDeviceAddress;    // Direct memory pointers in shaders;

    // --- PRESENTATION & SWAPCHAIN ---
    Bool8 PresentModeFifoLatestReady; // (VK_KHR_present_mode_fifo_latest_ready)

    // Push Constant Limits
    UInt32 MinPushConstantsSize;
} VulkanPhysicalDevicePreferences;

/** @brief Contains swapchain support information and capabilities. */
typedef struct VulkanSwapchainSupportInfo {
    /** @brief The surface capabilities. */
    VkSurfaceCapabilitiesKHR Capabilities;
    /** @brief The number of available surface formats. */
    UInt32 FormatCount;
    /** @brief An array of the available surface formats. */
    VkSurfaceFormatKHR* Formats;
    /** @brief The number of available presentation modes. */
    UInt32 PresentModeCount;
    /** @brief An array of available presentation modes. */
    VkPresentModeKHR* PresentModes;
} VulkanSwapchainSupportInfo;

/**
 * @brief Represents a Vulkan-specific buffer.
 * Used to load data onto the GPU.
 */
typedef struct VulkanBuffer {
    /** @brief The total size of the buffer. */
    UInt64 TotalSize;
    /** @brief The handle to the internal buffer. */
    VkBuffer Handle;
    /** @brief The usage flags. */
    VkBufferUsageFlagBits Usage;
    /** @brief Indicates if the buffer's memory is currently locked. */
    Bool8 IsLocked;
    /** @brief The memory used by the buffer. */
    VkDeviceMemory Memory;
    /** @brief The index of the memory used by the buffer. */
    Int32 MemoryIndex;
    /** @brief The property flags for the memory used by the buffer. */
    UInt32 MemoryPropertyFlags;
} VulkanBuffer;

/**
 * @brief A representation of both the physical and logical
 * Vulkan devices. Also contains handles to queues, command pools,
 * and various properties of the devices.
 */
typedef struct VulkanDevice {
    /** @brief The physical device. This is a representation of the GPU itself. */
    VkPhysicalDevice PhysicalDevice;
    /** @brief The logical device. This is the application's view of the device, used for most Vulkan operations. */
    VkDevice LogicalDevice;
    /** @brief The swapchain support info. */
    VulkanSwapchainSupportInfo SwapchainSupport;

    /** @brief The index of the graphics queue. */
    Int32 GraphicsQueueIndex;
    /** @brief The index of the present queue. */
    Int32 PresentQueueIndex;
    /** @brief The index of the transfer queue. */
    Int32 TransferQueueIndex;
    Int32 ComputeQueueIndex;

    /** @brief Indicates if the device supports a memory type that is both host visible and device local. */
    Bool8 SupportsDeviceLocalHostVisible;

    /** @brief A handle to a graphics queue. */
    VkQueue GraphicsQueue;
    /** @brief A handle to a present queue. */
    VkQueue PresentQueue;
    /** @brief A handle to a transfer queue. */
    VkQueue TransferQueue;
    VkQueue ComputeQueue;

    /** @brief A handle to a command pool for graphics operations. */
    VkCommandPool GraphicsCommandPool;

    /** @brief The physical device properties. */
    VkPhysicalDeviceProperties Properties;
    /** @brief The physical device features. */
    VkPhysicalDeviceFeatures Features;
    /** @brief The physical device memory properties. */
    VkPhysicalDeviceMemoryProperties Memory;

    /** @brief The chosen supported depth format. */
    VkFormat DepthFormat;
} VulkanDevice;

/**
 * @brief A representation of a Vulkan image. This can be thought
 * of as a texture. Also contains the view and memory used by
 * the internal image.
 */
typedef struct VulkanImage {
    /** @brief The handle to the internal image object. */
    VkImage Handle;
    /** @brief The memory used by the image. */
    VkDeviceMemory Memory;
    /** @brief The view for the image, which is used to access the image. */
    VkImageView View;
    /** @brief The image width. */
    UInt32 Width;
    /** @brief The image height. */
    UInt32 Height;
} VulkanImage;

/** @brief Represents the possible states of a renderpass. */
typedef enum VulkanRenderPassState {
    /** @brief The renderpass is ready to begin. */
    VULKAN_RENDER_PASS_STATE_READY,
    /** @brief The renderpass is currently being recorded to. */
    VULKAN_RENDER_PASS_STATE_RECORDING,
    /** @brief The renderpass is currently active. */
    VULKAN_RENDER_PASS_STATE_IN_RENDER_PASS,
    /** @brief The renderpass has ended recording. */
    VULKAN_RENDER_PASS_STATE_RECORDING_ENDED,
    /** @brief The renderpass has been submitted to the queue. */
    VULKAN_RENDER_PASS_STATE_SUBMITTED,
    /** @brief The renderpass is not allocated. */
    VULKAN_RENDER_PASS_STATE_NOT_ALLOCATED
} VulkanRenderPassState;

// Unused since this engine is Vulkan 1.3+, however i don't feel like going off course
// from the kohi game engine series, i am trying to get this to work
/**
 * @brief A representation of the Vulkan renderpass.
 */
typedef struct VulkanRenderpass {
    /** @brief The internal renderpass handle. */
    VkRenderPass Handle;
    /** @brief The current render area of the renderpass. */
    Vec4 RenderArea;
    /** @brief The clear colour used for this renderpass. */
    Vec4 ClearColor;

    /** @brief The clear flags for this renderpass. */
    UInt8 ClearFlags;
    /** @brief Indicates if there is a previous renderpass. */
    Bool8 HasPreviousPass;
    /** @brief Indicates if there is a next renderpass. */
    Bool8 HasNextPass;

    /** @brief The depth clear value. */
    Float32 Depth;
    /** @brief The stencil clear value. */
    UInt32 Stencil;

    /** @brief Indicates renderpass state. */
    VulkanRenderPassState State;
} VulkanRenderpass;

/**
 * @brief Representation of the Vulkan swapchain.
 */
typedef struct VulkanSwapchain {
    /** @brief The swapchain image format. */
    VkSurfaceFormatKHR ImageFormat;
    /** @brief CHecks if the provided format is UNORM */
    Bool8 IsFormatUnorm;
    /** 
     * @brief The maximum number of "images in flight" (images simultaneously being rendered to).
     * Typically one less than the total number of images available.
     */
    UInt8 MaxFramesInFlight;
    /** @brief The swapchain internal handle. */
    VkSwapchainKHR Handle;
    /** @brief The number of swapchain images. */
    UInt32 ImageCount;
    /** @brief An array of swapchain images. */
    VkImage* Images;
    /** @brief An array of swapchain image views for the swapchain images. */
    VkImageView* Views;

    /** @brief Framebuffers used for on-screen rendering, one per frame. */
    VkFramebuffer Framebuffers[3];

    /** @brief The depth image attachment. */
    VulkanImage DepthAttachment;
} VulkanSwapchain;

/**
 * @brief Represents all of the available states that
 * a command buffer can be in.
 */
typedef enum VulkanCommandBufferState {
    /** @brief The command buffer is ready to begin. */
    VULKAN_COMMAND_BUFFER_STATE_READY,
    /** @brief The command buffer is currently being recorded to. */
    VULKAN_COMMAND_BUFFER_STATE_RECORDING,
    /** @brief The command buffer is currently active. */
    VULKAN_COMMAND_BUFFER_STATE_IN_RENDER_PASS,
    /** @brief The command buffer has ended recording. */
    VULKAN_COMMAND_BUFFER_STATE_RECORDING_ENDED,
    /** @brief The command buffer has been submitted to the queue. */
    VULKAN_COMMAND_BUFFER_STATE_SUBMITTED,
    /** @brief The command buffer is not allocated. */
    VULKAN_COMMAND_BUFFER_STATE_NOT_ALLOCATED
} VulkanCommandBufferState;

/**
 * @brief Represents a Vulkan-specific command buffer, which
 * holds a list of commands and is submitted to a queue
 * for execution.
 */
typedef struct VulkanCommandBuffer {
    /** @brief The internal command buffer handle. */
    VkCommandBuffer Handle;

    /** @brief Command buffer state. */
    VulkanCommandBufferState State;
} VulkanCommandBuffer;

/**
 * @brief Represents a single shader stage.
 */
typedef struct VulkanShaderStage {
    /** @brief The shader module creation info. */
    VkShaderModuleCreateInfo CreateInfo;
    /** @brief The internal shader module handle. */
    VkShaderModule Handle;
    /** @brief The pipeline shader stage creation info. */
    VkPipelineShaderStageCreateInfo ShaderStageCreateInfo;
} VulkanShaderStage;

/**
 * @brief Holds a Vulkan pipeline and its layout.
 */
typedef struct VulkanPipeline {
    /** @brief The internal pipeline handle. */
    VkPipeline Handle;
    /** @brief The pipeline layout. */
    VkPipelineLayout PipelineLayout;
} VulkanPipeline;

/** @brief The number of shader stages in the material shader. */
constexpr UInt32 MaterialShaderStageCount = 2;

/** 
 * @brief Represents a state for a given descriptor. This is used 
 * to determine when a descriptor needs updating. There is a state
 * per frame (with a max of 3).
 */
typedef struct VulkanDescriptorState{
    /** @brief The descriptor generation, per frame. */
    UInt32 Generations[3];
    /** @brief The identifier, per frame. Typically used for texture ids. */
    UInt32 IDs[3];
} VulkanDescriptorState;

/** @brief The number of descriptors present in the material shader. */
constexpr UInt32 VulkanMaterialShaderDescriptorCount = 2;
/** @brief The number of texture samplers present in the material shader. */
constexpr UInt32 VulkanMaterialShaderSamplerCount = 1;

/**
 * @brief Represents the state of a material instance. Each
 * material in the world is a material instance, so one per
 * material exists.
 */
typedef struct VulkanMaterialShaderInstanceState {
    /** @brief The descriptor sets for this material instance, one per frame. */
    VkDescriptorSet DescriptorSets[3];

    /** @brief A descriptor state per descriptor, which in turn handles frames. */
    VulkanDescriptorState DescriptorStates[VulkanMaterialShaderDescriptorCount];
} VulkanMaterialShaderInstanceState;

/** 
 * @brief Max number of material instances 
 * @todo TODO: make configurable
 */
constexpr UInt32 VulkanMaxMaterialCount = 1024;

/** 
 * @brief Max number of simultaneously uploaded geometries 
 * @todo TODO: make configurable
 */
constexpr UInt32 VulkanMaxGeometryCount = 4096;

/**
 * @brief Internal buffer data for geometry. This data gets loaded
 * directly into a buffer.
 */
typedef struct VulkanGeometryData {
    /** @brief The unique geometry identifier. */
    UInt32 ID;
    /** @brief The geometry generation. Incremented every time the geometry data changes. */
    UInt32 Generation;
    /** @brief The vertex count. */
    UInt32 VertexCount;
    /** @brief The size of each vertex. */
    UInt32 VertexElementSize;
    /** @brief The offset in bytes in the vertex buffer. */
    UInt32 VertexBufferOffset;
    /** @brief The index count. */
    UInt32 IndexCount;
    /** @brief The size of each index. */
    UInt32 IndexElementSize;
    /** @brief The offset in bytes in the index buffer. */
    UInt32 IndexBufferOffset;
} VulkanGeometryData;

/**
 * @brief Represents the global material shader uniform buffer data
 * that should be loaded into the global ubo buffer.
 * @note This must be padded out to 256 bytes because of the requirement
 * from some GPUs (I'm looking at you, nVidia) that UBO buffer elements be
 * at least 256 bytes apart.
 */
typedef struct VulkanMaterialShaderGlobalUBO {
    /** @brief The projection matrix. */
    Mat4 Projection;
    /** @brief The view matrix. */
    Mat4 View;
    /** @brief reserved for future use. */
    Mat4 MatrixReserved0;
    /** @brief reserved for future use. */
    Mat4 MatrixReserved1;
} VulkanMaterialShaderGlobalUBO;

/**
 * @brief Represents per-material instance uniform buffer data
 * that should be loaded into the local ubo buffer.
 * @note This must be padded out to 256 bytes because of the requirement
 * from some GPUs (I'm looking at you, nVidia) that UBO buffer elements be
 * at least 256 bytes apart.
 */
typedef struct VulkanMaterialShaderInstanceUBO {
    /** @brief The diffuse colour. */
    Vec4 BaseColor;
    /** @brief reserved for future use. */
    Vec4 Reserved0;
    /** @brief reserved for future use. */
    Vec4 Reserved1;
    /** @brief reserved for future use. */
    Vec4 Reserved2;
} VulkanMaterialShaderInstanceUBO;

/**
 * @brief A representation of the vulkan material shader.
 * Holds internal resources such as descriptors/layouts, ubos,
 * buffers, etc.
 */
typedef struct VulkanMaterialShader {
    /** @brief The shader stages. */
    VulkanShaderStage Stages[MaterialShaderStageCount];

    /** @brief The pipeline associated with this shader. */
    VulkanPipeline Pipeline;

    /** @brief The pool which global descriptors are allocated. */
    VkDescriptorPool GlobalDescriptorPool;
    /** @brief The global descriptor set layout. */
    VkDescriptorSetLayout GlobalDescriptorSetLayout;

    /** @brief Global descriptor sets. */
    VkDescriptorSet GlobalDescriptorSets[3];

    /** @brief Global uniform object, to be loaded into the global uniform buffer. */
    VulkanMaterialShaderGlobalUBO GlobalUBO;

    /** @brief Global uniform buffer. */
    VulkanBuffer GlobalUniformBuffer;

    /** @brief The pool from which material-instance descriptors are allocated. */
    VkDescriptorPool ObjectDescriptorPool;
    /** @brief The descriptor set layout for material instance descriptors. */
    VkDescriptorSetLayout ObjectDescriptorSetLayout;

    /** @brief Material instance uniform buffer. */
    VulkanBuffer ObjectUniformBuffer;
    /** @brief The current material instance index for linear allocations. */
    UInt32 ObjectUniformBufferIndex;

    /** @brief represents the usage of samplers. */
    TextureUse SamplerUses[VulkanMaterialShaderSamplerCount];

    /** @brief The material instance states for all material instances. */
    VulkanMaterialShaderInstanceState InstanceStates[VulkanMaxMaterialCount];

} VulkanMaterialShader;

/** @brief The number of shader stages in the UI shader. */
constexpr UInt32 UIShaderStageCount = 2;

/** @brief The number of descriptors present in the UI shader. */
constexpr UInt32 VulkanUIShaderDescriptorCount = 2;
/** @brief The number of texture samplers present in the UI shader. */
constexpr UInt32 VulkanUIShaderSamplerCount = 1;

/** 
 * @brief Max number of UI control instances 
 * @todo TODO: make configurable
 */
constexpr UInt32 VulkanMaxUICount = 1024;

/**
 * @brief Represents the state of a UI control instance.
 */
typedef struct VulkanUIShaderInstanceState {
    /** @brief The descriptor sets for this UI control instance. */
    VkDescriptorSet DescriptorSets[3];

    /** @brief A descriptor state per descriptor. */
    VulkanDescriptorState DescriptorStates[VulkanUIShaderDescriptorCount];
} VulkanUIShaderInstanceState;

/**
 * @brief Represents the global UI shader uniform buffer data.
 */
typedef struct VulkanUIShaderGlobalUBO {
    /** @brief The projection matrix. */
    Mat4 Projection;
    /** @brief The view matrix. */
    Mat4 View;
    /** @brief reserved for future use. */
    Mat4 MatrixReserved0;
    /** @brief reserved for future use. */
    Mat4 MatrixReserved1;
} VulkanUIShaderGlobalUBO;

/**
 * @brief Represents per-UI instance uniform buffer data.
 */
typedef struct VulkanUIShaderInstanceUBO {
    /** @brief The diffuse colour. */
    Vec4 BaseColor;
    /** @brief reserved for future use. */
    Vec4 Reserved0;
    /** @brief reserved for future use. */
    Vec4 Reserved1;
    /** @brief reserved for future use. */
    Vec4 Reserved2;
} VulkanUIShaderInstanceUBO;

/**
 * @brief A representation of the vulkan UI shader.
 */
typedef struct VulkanUIShader {
    /** @brief The shader stages. */
    VulkanShaderStage Stages[UIShaderStageCount];

    /** @brief The pipeline associated with this shader. */
    VulkanPipeline Pipeline;

    /** @brief The pool which global descriptors are allocated. */
    VkDescriptorPool GlobalDescriptorPool;
    /** @brief The global descriptor set layout. */
    VkDescriptorSetLayout GlobalDescriptorSetLayout;

    /** @brief Global descriptor sets. */
    VkDescriptorSet GlobalDescriptorSets[3];

    /** @brief Global uniform object. */
    VulkanUIShaderGlobalUBO GlobalUBO;

    /** @brief Global uniform buffer. */
    VulkanBuffer GlobalUniformBuffer;

    /** @brief The pool from which per-instance descriptors are allocated. */
    VkDescriptorPool ObjectDescriptorPool;
    /** @brief The descriptor set layout for per-instance descriptors. */
    VkDescriptorSetLayout ObjectDescriptorSetLayout;

    /** @brief Material instance uniform buffer. */
    VulkanBuffer ObjectUniformBuffer;
    /** @brief The current material instance index for linear allocations. */
    UInt32 ObjectUniformBufferIndex;

    /** @brief represents the usage of samplers. */
    TextureUse SamplerUses[VulkanUIShaderSamplerCount];

    /** @brief The material instance states for all material instances. */
    VulkanUIShaderInstanceState InstanceStates[VulkanMaxUICount];

} VulkanUIShader;

/**
 * @brief The overall Vulkan context for the backend. Holds and maintains
 * global renderer backend state, Vulkan instance, etc.
 */
typedef struct VulkanContext {
    /** @brief The time in seconds since the last frame. */
    Float32 FrameDeltaTime;

    /** @brief The handle to the internal Vulkan instance. */
    VkInstance Instance;
    /** @brief The internal Vulkan allocator. */
    VkAllocationCallbacks* Allocator;
    /** @brief The internal Vulkan surface for the window to be drawn to. */
    VkSurfaceKHR Surface;
    /** @brief The Vulkan device. */
    VulkanDevice Device;
    VulkanPhysicalDevicePreferences Preferences;

    /** @brief The swapchain. */
    VulkanSwapchain Swapchain;
    /** @brief The main world renderpass. */
    VulkanRenderpass MainRenderpass;
    /** @brief The UI renderpass. */
    VulkanRenderpass UIRenderpass;

    /** @brief The object vertex buffer. */
    VulkanBuffer ObjectVertexBuffer;
    /** @brief The object index buffer. */
    VulkanBuffer ObjectIndexBuffer;

    /** @brief The graphics command buffers, one per frame. */
    VulkanCommandBuffer* GraphicsCommandBuffers;

    /** @brief The semaphores used to indicate image availability, one per frame. */
    VkSemaphore* ImageAvailableSemaphores;

    /** @brief The semaphores used to indicate queue availability, one per frame. */
    VkSemaphore* QueueCompleteSemaphores;

    /** @brief The current number of in-flight fences. */
    UInt32 InFlightFenceCount;
    /** @brief The in-flight fences. */
    VkFence InFlightFences[2];

    /** @brief Holds pointers to fences which exist and are owned elsewhere, one per frame. */
    VkFence* ImagesInFlight[3];

    /** @brief The current image index. */
    UInt32 ImageIndex;
    /** @brief The current frame. */
    UInt32 CurrentFrame;

    Bool8 RecreateSwapchain;

    /** @brief The framebuffer's current width. */
    UInt32 FramebufferWidth;
    /** @brief The framebuffer's current height. */
    UInt32 FramebufferHeight;

    UInt32 FramebufferCurrentGeneration;
    UInt32 FramebufferLastGeneration;

    /** @brief The material shader. */
    VulkanMaterialShader MaterialShader;
    /** @brief The UI shader. */
    VulkanUIShader UIShader;

    /** @brief The geometry vertex buffer offset. */
    UInt64 GeometryVertexOffset;
    /** @brief The geometry index buffer offset. */
    UInt64 GeometryIndexOffset;

    /** @brief A collection of loaded geometries. */
    VulkanGeometryData Geometries[VulkanMaxGeometryCount];

    /** @brief Framebuffers used for world rendering, one per frame. */
    VkFramebuffer WorldFramebuffers[3];

    /** 
     * @brief A function pointer to find a memory index of the given type and with the given properties.
     * @param TypeFilter The types of memory to search for.
     * @param PropertyFlags The required properties which must be present.
     * @returns The index of the found memory type. Returns -1 if not found.
     */
    Int32 (*FindMemoryIndex)(UInt32 TypeFilter, UInt32 PropertyFlags);
    
#if defined(_DEBUG)
    /** @brief The debug messenger, if active. */
    VkDebugUtilsMessengerEXT DebugMessenger;
#endif
} VulkanContext;

/**
 * @brief Represents Vulkan-specific texture data.
 */
typedef struct VulkanTextureData {
    /** @brief The internal Vulkan image. */
    VulkanImage Image;
    /** @brief The internal image sampler. */
    VkSampler Sampler;
} VulkanTextureData;

static inline Bool8 VulkanFormatHasStencil(VkFormat Format)
{
    return Format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
           Format == VK_FORMAT_D24_UNORM_S8_UINT   ||
           Format == VK_FORMAT_D16_UNORM_S8_UINT;
}

#undef UINT8_MAX
#undef UINT16_MAX
#undef UINT32_MAX
#undef UINT64_MAX

#undef INT8_MAX
#undef INT16_MAX
#undef INT32_MAX
#undef INT64_MAX

#undef INT8_MIN
#undef INT16_MIN
#undef INT32_MIN
#undef INT64_MIN

// --- Unsigned Integers Maximums ---
#define UINT8_MAX   255U
#define UINT16_MAX  65535U
#define UINT32_MAX  4294967295U
#define UINT64_MAX  0xffffffffffffffffULL

// --- Signed Integers Maximums ---
#define INT8_MAX    127
#define INT16_MAX   32767
#define INT32_MAX   2147483647
#define INT64_MAX   9223372036854775807LL

// --- Signed Integers Minimums ---
#define INT8_MIN    (-127 - 1)
#define INT16_MIN   (-32767 - 1)
#define INT32_MIN   (-2147483647 - 1)
#define INT64_MIN   (-9223372036854775807LL - 1)