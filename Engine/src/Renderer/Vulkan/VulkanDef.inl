#pragma once

#include <vulkan/vulkan.h>

#include "Defines.h"

#include "Core/Logger.h"
#include "Core/Asserts.h"

#define Gigabytes(value) ((value) * 1024ULL * 1024ULL * 1024ULL - 1)
#define Megabytes(value) ((value) * 1024ULL * 1024ULL)

// Checks the given expression's return value against VK_SUCCESS.
#define VK_CHECK(expr)                                                                                          \
do {                                                                                                            \
        VkResult _result = (expr);                                                                              \
        if ((_result) < VK_SUCCESS) {                                                                           \
            FLERROR("Vulkan Error %i executing: %s (File: %s, Line: %d)", _result, #expr, __FILE__, __LINE__);  \
            FASSERT_MSG(false, "Vulkan runtime crash!");               \
        }                                                                                                       \
        else if (_result == VK_SUBOPTIMAL_KHR) {                                                                \
            FLWARN("Vulkan Suboptimal Swapchain detected (File: %s, Line: %d)", __FILE__, __LINE__);            \
        }                                                                                                       \
        else if (_result == VK_INCOMPLETE ||                                                                    \
                 _result == VK_PIPELINE_BINARY_MISSING_KHR ||                                                   \
                 _result == VK_INCOMPATIBLE_SHADER_BINARY_EXT) {                                                \
            FLDEBUG("Vulkan Cache/Query Status %i executing: %s", _result, #expr);                              \
        }                                                                                                       \
        else if (_result != VK_SUCCESS && _result != VK_NOT_READY && _result != VK_TIMEOUT) {                   \
            FLDEBUG("Vulkan Status Flag %i executing: %s", _result, #expr);                                     \
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
    
    // --- LIMITS ---
    UInt32 MinimumPushConstantsSize;
} VulkanPhysicalDevicePreferences;

typedef struct VulkanSwapchainSupportInfo {
    VkSurfaceCapabilitiesKHR Capabilities;
    UInt32 FormatCount;
    VkSurfaceFormatKHR* Formats;
    UInt32 PresentModeCount;
    VkPresentModeKHR* PresentModes;
} VulkanSwapchainSupportInfo;

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

typedef struct VulkanContext {
    VkInstance Instance;
    VkAllocationCallbacks* Allocator;
    VkSurfaceKHR Surface;
    VulkanDevice Device;
    VulkanPhysicalDevicePreferences Preferences;

    VulkanSwapchain Swapchain;
    VulkanRenderpass MainRenderpass;

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