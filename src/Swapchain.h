#pragma once

#include "Device.h"

// vulkan headers
#include <vulkan/vulkan.h>

// std lib headers
#include <string>
#include <vector>

namespace FractalEngine
{

  class FractalSwapChain
  {
  public:
    static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

    FractalSwapChain(FractalDevice &DeviceRef, VkExtent2D windowExtent);
    ~FractalSwapChain();

    FractalSwapChain(const FractalSwapChain &) = delete;
    void operator=(const FractalSwapChain &) = delete;

    VkFramebuffer getFrameBuffer(int index) { return SwapChainFrameBuffers[index]; }
    VkRenderPass getRenderPass() { return renderPass; }
    VkImageView getImageView(int index) { return SwapChainImageViews[index]; }
    size_t imageCount() { return SwapChainImages.size(); }
    VkFormat getSwapChainImageFormat() { return SwapChainImageFormat; }
    VkExtent2D getSwapChainExtent() { return SwapChainExtent; }
    uint32_t width() { return SwapChainExtent.width; }
    uint32_t height() { return SwapChainExtent.height; }

    float extentAspectRatio()
    {
      return static_cast<float>(SwapChainExtent.width) / static_cast<float>(SwapChainExtent.height);
    }
    VkFormat findDepthFormat();

    VkResult acquireNextImage(uint32_t *ImageIndex);
    VkResult submitCommandBuffers(const VkCommandBuffer *Buffers, uint32_t *ImageIndex);

  private:
    void createSwapChain();
    void createImageViews();
    void createDepthResources();
    void createRenderPass();
    void createFrameBuffers();
    void createSyncObjects();

    // Helper functions
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(
        const std::vector<VkSurfaceFormatKHR> &AvailableFormats);
    VkPresentModeKHR chooseSwapPresentMode(
        const std::vector<VkPresentModeKHR> &AvailablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &Capabilities);

    VkFormat SwapChainImageFormat;
    VkExtent2D SwapChainExtent;

    std::vector<VkFramebuffer> SwapChainFrameBuffers;
    VkRenderPass renderPass;

    std::vector<VkImage> DepthImages;
    std::vector<VkDeviceMemory> DepthImageMemorys;
    std::vector<VkImageView> DepthImageViews;
    std::vector<VkImage> SwapChainImages;
    std::vector<VkImageView> SwapChainImageViews;

    FractalDevice &Device;
    VkExtent2D windowExtent;

    VkSwapchainKHR SwapChain;

    std::vector<VkSemaphore> ImageAvailableSemaphores;
    std::vector<VkSemaphore> RenderFinishedSemaphores;
    std::vector<VkFence> InFlightFences;
    std::vector<VkFence> ImagesInFlight;
    size_t CurrentFrame = 0;
  };

} // namespace lve
