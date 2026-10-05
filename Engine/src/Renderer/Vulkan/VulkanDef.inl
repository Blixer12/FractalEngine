#pragma once

#include <vulkan/vulkan.h>
#include "Renderer/CrystalDef.inl"

#include "Defines.h"

#include "Core/Logger.h"
#include "Core/Asserts.h"

#define Gigabytes(value) ((value) * 1024ULL * 1024ULL * 1024ULL - 1)
#define Megabytes(value) ((value) * 1024ULL * 1024ULL)

// Checks the given expression's return value against VK_SUCCESS.
#define VK_CHECK(expr)                                                                                          \
do {                                                                                                            \
        VkResult _Result = (expr);                                                                              \
        if ((_Result) < VK_SUCCESS) {                                                                           \
            FLERROR("Vulkan Error %i executing: %s (File: %s, Line: %d)", _Result, #expr, __FILE__, __LINE__);  \
            FASSERT_MSG(false, "Vulkan runtime crash!");                                                        \
        }                                                                                                       \
        else if (_Result == VK_SUBOPTIMAL_KHR) {                                                                \
            FLWARN("Vulkan Suboptimal Swapchain detected (File: %s, Line: %d)", __FILE__, __LINE__);            \
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
} VulkanPhysicalDevicePreferences;

typedef struct VulkanSwapchainSupportInfo {
    VkSurfaceCapabilitiesKHR Capabilities;
    UInt32 FormatCount;
    VkSurfaceFormatKHR* Formats;
    UInt32 PresentModeCount;
    VkPresentModeKHR* PresentModes;
} VulkanSwapchainSupportInfo;

typedef struct VulkanBuffer {
    UInt64 TotalSize;
    VkBuffer Handle;
    VkBufferUsageFlagBits Usage;
    Bool8 IsLocked;
    VkDeviceMemory Memory;
    Int32 MemoryIndex;
    UInt32 MemoryPropertyFlags;
} VulkanBuffer;

typedef struct VulkanDevice {
    VkPhysicalDevice PhysicalDevice;
    VkDevice LogicalDevice;
    VulkanSwapchainSupportInfo SwapchainSupport;

    Int32 GraphicsQueueIndex;
    Int32 PresentQueueIndex;
    Int32 TransferQueueIndex;
    Int32 ComputeQueueIndex;

    VkQueue GraphicsQueue;
    VkQueue PresentQueue;
    VkQueue TransferQueue;
    VkQueue ComputeQueue;

    VkCommandPool GraphicsCommandPool;

    VkPhysicalDeviceProperties Properties;
    VkPhysicalDeviceFeatures Features;
    VkPhysicalDeviceMemoryProperties Memory;

    VkFormat DepthFormat;
} VulkanDevice;

typedef struct VulkanImage {
    VkImage Handle;
    VkDeviceMemory Memory;
    VkImageView View;
    UInt32 Width;
    UInt32 Height;
} VulkanImage;

typedef enum VulkanRenderPassState {
    VULKAN_RENDER_PASS_STATE_READY,
    VULKAN_RENDER_PASS_STATE_RECORDING,
    VULKAN_RENDER_PASS_STATE_IN_RENDER_PASS,
    VULKAN_RENDER_PASS_STATE_RECORDING_ENDED,
    VULKAN_RENDER_PASS_STATE_SUBMITTED,
    VULKAN_RENDER_PASS_STATE_NOT_ALLOCATED
} VulkanRenderPassState;

// Unused since this engine is Vulkan 1.3+, however i don't feel like going off course
// from the kohi game engine series, i am trying to get this to work
typedef struct VulkanRenderpass {
    VkRenderPass Handle;
    Float32 X, Y, W, H;
    Float32 R, G, B, A;

    Float32 Depth;
    UInt32 Stencil;

    VulkanRenderPassState State;
} VulkanRenderpass;

typedef struct VulkanFramebuffer {
    VkFramebuffer Handle;
    UInt32 AttachmentCount;
    VkImageView* Attachments;
    VulkanRenderpass* Renderpass;
} VulkanFramebuffer;

typedef struct VulkanSwapchain {
    VkSurfaceFormatKHR ImageFormat;
    Bool8 IsFormatUnorm;
    UInt8 MaxFramesInFlight;
    VkSwapchainKHR Handle;
    UInt32 ImageCount;
    VkImage* Images;
    VkImageView* Views;

    VulkanFramebuffer* Framebuffers;

    VulkanImage DepthAttachment;
} VulkanSwapchain;

typedef enum VulkanCommandBufferState {
    VULKAN_COMMAND_BUFFER_STATE_READY,
    VULKAN_COMMAND_BUFFER_STATE_RECORDING,
    VULKAN_COMMAND_BUFFER_STATE_IN_RENDER_PASS,
    VULKAN_COMMAND_BUFFER_STATE_RECORDING_ENDED,
    VULKAN_COMMAND_BUFFER_STATE_SUBMITTED,
    VULKAN_COMMAND_BUFFER_STATE_NOT_ALLOCATED
} VulkanCommandBufferState;

typedef struct VulkanCommandBuffer {
    VkCommandBuffer Handle;
    VulkanCommandBufferState State;
} VulkanCommandBuffer;

typedef struct VulkanFence {
    VkFence Handle;
    Bool8 IsSignaled;
} VulkanFence;

typedef struct VulkanShaderStage {
    VkShaderModuleCreateInfo CreateInfo;
    VkShaderModule Handle;
    VkPipelineShaderStageCreateInfo ShaderStageCreateInfo;
} VulkanShaderStage;

typedef struct VulkanPipeline {
    VkPipeline Handle;
    VkPipelineLayout PipelineLayout;
} VulkanPipeline;

#define OBJECT_SHADER_STAGE_COUNT 2
typedef struct VulkanObjectShader {
    // vertex, fragment
    VulkanShaderStage Stages[OBJECT_SHADER_STAGE_COUNT];

    VulkanPipeline Pipeline;

    VkDescriptorPool GlobalDescriptorPool;
    VkDescriptorSetLayout GlobalDescriptorSetLayout;

    // One pe-frame, we are triple buffering so 3
    VkDescriptorSet GlobalDescriptorSets[3];

    // Global Uniform Object
    GlobalUniformObject GlobalUBO;

    // Global Uniform Buffer
    VulkanBuffer GlobalUniformBuffer;

} VulkanObjectShader;


typedef struct VulkanContext {
    VkInstance Instance;
    VkAllocationCallbacks* Allocator;
    VkSurfaceKHR Surface;
    VulkanDevice Device;
    VulkanPhysicalDevicePreferences Preferences;

    VulkanSwapchain Swapchain;
    VulkanRenderpass MainRenderpass;

    VulkanBuffer ObjectVertexBuffer;
    VulkanBuffer ObjectIndexBuffer;

    // This variable is a Vector used to track the amount of Command Buffers in use
    VulkanCommandBuffer* GraphicsCommandBuffers;

    // This variable is a Vector that tracks if a swapchain image is available for rendering
    VkSemaphore* ImageAvailableSemaphores;

    // This variable is a Vector that tracks when any CommandBuffer Queue is complete
    VkSemaphore* QueueCompleteSemaphores;

    UInt32 InFlightFenceCount;
    VulkanFence* InFlightFences;

    // A Vector tracking pointers to fences currently in use by an active swapchain image index
    VulkanFence** ImagesInFlight;

    UInt32 ImageIndex;
    UInt32 CurrentFrame;

    Bool8 RecreateSwapchain;

    UInt32 FramebufferWidth;
    UInt32 FramebufferHeight;
    Bool8 WindowResized;

    VulkanObjectShader ObjectShader;

    UInt64 GeometryVertexOffset;
    UInt64 GeometryIndexOffset;

    Int32 (*FindMemoryIndex)(UInt32 TypeFilter, UInt32 PropertyFlags);
    
    #if defined(_DEBUG)
    VkDebugUtilsMessengerEXT DebugMessenger;
    #endif
} VulkanContext;

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