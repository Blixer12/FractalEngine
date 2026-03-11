#include "Swapchain.h"

// std
#include <array>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <set>
#include <stdexcept>

namespace FractalEngine
{

  FractalSwapChain::FractalSwapChain(FractalDevice &DeviceRef, VkExtent2D extent)
      : Device{DeviceRef}, windowExtent{extent}
  {
    createSwapChain();
    createImageViews();
    createRenderPass();
    createDepthResources();
    createFrameBuffers();
    createSyncObjects();
  }

  FractalSwapChain::~FractalSwapChain()
  {

    vkDeviceWaitIdle(Device.device());

    for (auto framebuffer : SwapChainFrameBuffers)
    {
      vkDestroyFramebuffer(Device.device(), framebuffer, nullptr);
    }

    for (auto imageView : SwapChainImageViews)
    {
      vkDestroyImageView(Device.device(), imageView, nullptr);
    }
    SwapChainImageViews.clear();

    for (size_t i = 0; i < DepthImages.size(); i++)
    {
      vkDestroyImageView(Device.device(), DepthImageViews[i], nullptr);
      vkDestroyImage(Device.device(), DepthImages[i], nullptr);
      vkFreeMemory(Device.device(), DepthImageMemorys[i], nullptr);
    }

    vkDestroyRenderPass(Device.device(), renderPass, nullptr);

    if (SwapChain != nullptr)
    {
      vkDestroySwapchainKHR(Device.device(), SwapChain, nullptr);
      SwapChain = nullptr;
    }

    // cleanup synchronization objects
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
      vkDestroySemaphore(Device.device(), ImageAvailableSemaphores[i], nullptr);
      vkDestroyFence(Device.device(), InFlightFences[i], nullptr);
    }

    for (size_t i = 0; i < RenderFinishedSemaphores.size(); i++)
    {
      vkDestroySemaphore(Device.device(), RenderFinishedSemaphores[i], nullptr);
    }
  }

  VkResult FractalSwapChain::acquireNextImage(uint32_t *ImageIndex)
  {
    vkWaitForFences(
        Device.device(),
        1,
        &InFlightFences[CurrentFrame],
        VK_TRUE,
        std::numeric_limits<uint64_t>::max());

    VkResult result = vkAcquireNextImageKHR(
        Device.device(),
        SwapChain,
        std::numeric_limits<uint64_t>::max(),
        ImageAvailableSemaphores[CurrentFrame], // must be a not signaled semaphore
        VK_NULL_HANDLE,
        ImageIndex);

    return result;
  }

  VkResult FractalSwapChain::submitCommandBuffers(
      const VkCommandBuffer *Buffers, uint32_t *ImageIndex)
  {
    if (ImagesInFlight[*ImageIndex] != VK_NULL_HANDLE)
    {
      vkWaitForFences(Device.device(), 1, &ImagesInFlight[*ImageIndex], VK_TRUE, UINT64_MAX);
    }
    ImagesInFlight[*ImageIndex] = InFlightFences[CurrentFrame];

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {ImageAvailableSemaphores[CurrentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = Buffers;

    VkSemaphore signalSemaphores[] = {RenderFinishedSemaphores[*ImageIndex]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(Device.device(), 1, &InFlightFences[CurrentFrame]);
    if (vkQueueSubmit(Device.graphicsQueue(), 1, &submitInfo, InFlightFences[CurrentFrame]) !=
        VK_SUCCESS)
    {
      throw std::runtime_error("failed to submit draw command buffer!");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR SwapChains[] = {SwapChain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = SwapChains;

    presentInfo.pImageIndices = ImageIndex;

    auto result = vkQueuePresentKHR(Device.presentQueue(), &presentInfo);

    CurrentFrame = (CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;

    return result;
  }

  void FractalSwapChain::createSwapChain()
  {
    SwapChainSupportDetails SwapChainSupport = Device.getSwapChainSupport();

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(SwapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(SwapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(SwapChainSupport.Capabilities);

    uint32_t imageCount = SwapChainSupport.Capabilities.minImageCount + 1;
    if (SwapChainSupport.Capabilities.maxImageCount > 0 &&
        imageCount > SwapChainSupport.Capabilities.maxImageCount)
    {
      imageCount = SwapChainSupport.Capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = Device.surface();

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = Device.findPhysicalQueueFamilies();
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

    if (indices.graphicsFamily != indices.presentFamily)
    {
      createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
      createInfo.queueFamilyIndexCount = 2;
      createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
      createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
      createInfo.queueFamilyIndexCount = 0;     // Optional
      createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = SwapChainSupport.Capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;

    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(Device.device(), &createInfo, nullptr, &SwapChain) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create swap chain!");
    }

    // we only specified a minimum number of images in the swap chain, so the implementation is
    // allowed to create a swap chain with more. That's why we'll first query the final number of
    // images with vkGetSwapChainImagesKHR, then resize the container and finally call it again to
    // retrieve the handles.
    vkGetSwapchainImagesKHR(Device.device(), SwapChain, &imageCount, nullptr);
    SwapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(Device.device(), SwapChain, &imageCount, SwapChainImages.data());

    SwapChainImageFormat = surfaceFormat.format;
    SwapChainExtent = extent;
  }

  void FractalSwapChain::createImageViews()
  {
    SwapChainImageViews.resize(SwapChainImages.size());
    for (size_t i = 0; i < SwapChainImages.size(); i++)
    {
      VkImageViewCreateInfo viewInfo{};
      viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      viewInfo.image = SwapChainImages[i];
      viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      viewInfo.format = SwapChainImageFormat;
      viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
      viewInfo.subresourceRange.baseMipLevel = 0;
      viewInfo.subresourceRange.levelCount = 1;
      viewInfo.subresourceRange.baseArrayLayer = 0;
      viewInfo.subresourceRange.layerCount = 1;

      if (vkCreateImageView(Device.device(), &viewInfo, nullptr, &SwapChainImageViews[i]) !=
          VK_SUCCESS)
      {
        throw std::runtime_error("failed to create texture image view!");
      }
    }
  }

  void FractalSwapChain::createRenderPass()
  {
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat();
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format = getSwapChainImageFormat();
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass = {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.srcAccessMask = 0;
    dependency.srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstSubpass = 0;
    dependency.dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask =
        VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    std::array<VkAttachmentDescription, 2> attachments = {colorAttachment, depthAttachment};
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(Device.device(), &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
    {
      throw std::runtime_error("failed to create render pass!");
    }
  }

  void FractalSwapChain::createFrameBuffers()
  {
    SwapChainFrameBuffers.resize(imageCount());
    for (size_t i = 0; i < imageCount(); i++)
    {
      std::array<VkImageView, 2> attachments = {SwapChainImageViews[i], DepthImageViews[i]};

      VkExtent2D SwapChainExtent = getSwapChainExtent();
      VkFramebufferCreateInfo framebufferInfo = {};
      framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
      framebufferInfo.renderPass = renderPass;
      framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
      framebufferInfo.pAttachments = attachments.data();
      framebufferInfo.width = SwapChainExtent.width;
      framebufferInfo.height = SwapChainExtent.height;
      framebufferInfo.layers = 1;

      if (vkCreateFramebuffer(
              Device.device(),
              &framebufferInfo,
              nullptr,
              &SwapChainFrameBuffers[i]) != VK_SUCCESS)
      {
        throw std::runtime_error("failed to create framebuffer!");
      }
    }
  }

  void FractalSwapChain::createDepthResources()
  {
    VkFormat depthFormat = findDepthFormat();
    VkExtent2D SwapChainExtent = getSwapChainExtent();

    DepthImages.resize(imageCount());
    DepthImageMemorys.resize(imageCount());
    DepthImageViews.resize(imageCount());

    for (int i = 0; i < DepthImages.size(); i++)
    {
      VkImageCreateInfo imageInfo{};
      imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
      imageInfo.imageType = VK_IMAGE_TYPE_2D;
      imageInfo.extent.width = SwapChainExtent.width;
      imageInfo.extent.height = SwapChainExtent.height;
      imageInfo.extent.depth = 1;
      imageInfo.mipLevels = 1;
      imageInfo.arrayLayers = 1;
      imageInfo.format = depthFormat;
      imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
      imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
      imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
      imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
      imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
      imageInfo.flags = 0;

      Device.createImageWithInfo(
          imageInfo,
          VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
          DepthImages[i],
          DepthImageMemorys[i]);

      VkImageViewCreateInfo viewInfo{};
      viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
      viewInfo.image = DepthImages[i];
      viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
      viewInfo.format = depthFormat;
      viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
      viewInfo.subresourceRange.baseMipLevel = 0;
      viewInfo.subresourceRange.levelCount = 1;
      viewInfo.subresourceRange.baseArrayLayer = 0;
      viewInfo.subresourceRange.layerCount = 1;

      if (vkCreateImageView(Device.device(), &viewInfo, nullptr, &DepthImageViews[i]) != VK_SUCCESS)
      {
        throw std::runtime_error("failed to create texture image view!");
      }
    }
  }

  void FractalSwapChain::createSyncObjects()
  {
    ImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    RenderFinishedSemaphores.resize(imageCount());
    InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    ImagesInFlight.resize(imageCount(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
      if (vkCreateSemaphore(Device.device(), &semaphoreInfo, nullptr, &ImageAvailableSemaphores[i]) !=
              VK_SUCCESS ||
          vkCreateFence(Device.device(), &fenceInfo, nullptr, &InFlightFences[i]) != VK_SUCCESS)
      {
        throw std::runtime_error("failed to create synchronization objects for a frame!");
      }
    }

    for (size_t i = 0; i < imageCount(); i++)
    {
      if (vkCreateSemaphore(Device.device(), &semaphoreInfo, nullptr, &RenderFinishedSemaphores[i]) != VK_SUCCESS)
      {
        throw std::runtime_error("failed to create Semaphore syncronization for a frame!");
      }
    }
  }

  VkSurfaceFormatKHR FractalSwapChain::chooseSwapSurfaceFormat(
      const std::vector<VkSurfaceFormatKHR> &AvailableFormats)
  {
    for (const auto &AvailableFormat : AvailableFormats)
    {
      if (AvailableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
          AvailableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
      {
        return AvailableFormat;
      }
    }

    return AvailableFormats[0];
  }

  VkPresentModeKHR FractalSwapChain::chooseSwapPresentMode(
      const std::vector<VkPresentModeKHR> &AvailablePresentModes)
  {

    for (const auto &AvailablePresentMode : AvailablePresentModes)
    {
      if (AvailablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
      {
        std::cout << "Present mode: Mailbox" << std::endl;
        return AvailablePresentMode;
      }
    }

    // for (const auto &AvailablePresentMode : AvailablePresentModes) {
    //   if (AvailablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
    //     std::cout << "Present mode: Immediate" << std::endl;
    //     return AvailablePresentMode;
    //   }
    // }

    std::cout << "Present mode: V-Sync" << std::endl;

    return VK_PRESENT_MODE_FIFO_KHR;
  }

  VkExtent2D FractalSwapChain::chooseSwapExtent(const VkSurfaceCapabilitiesKHR &Capabilities)
  {
    if (Capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
      return Capabilities.currentExtent;
    }
    else
    {
      VkExtent2D actualExtent = windowExtent;
      actualExtent.width = std::max(
          Capabilities.minImageExtent.width,
          std::min(Capabilities.maxImageExtent.width, actualExtent.width));
      actualExtent.height = std::max(
          Capabilities.minImageExtent.height,
          std::min(Capabilities.maxImageExtent.height, actualExtent.height));

      std::cout << "SwapChain extent: " << SwapChainExtent.width << "x" << SwapChainExtent.height << std::endl;

      return actualExtent;
    }
  }

  VkFormat FractalSwapChain::findDepthFormat()
  {
    return Device.findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
  }

} // namespace FractalEngine