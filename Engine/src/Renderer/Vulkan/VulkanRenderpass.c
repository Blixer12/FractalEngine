#include "VulkanRenderpass.h"

#include "Core/Memory.h"

void VulkanRenderpassCreate(
    VulkanContext* Context, 
    VulkanRenderpass* Renderpass,
    Float32 X, Float32 Y, Float32 W, Float32 H,
    Float32 R, Float32 G, Float32 B, Float32 A,
    Float32 Depth,
    UInt32 Stencil) {
    
    Renderpass->X = X;
    Renderpass->Y = Y;
    Renderpass->W = W;
    Renderpass->H = H;

    Renderpass->R = R;
    Renderpass->G = G;
    Renderpass->B = B;
    Renderpass->A = A;

    Renderpass->Depth = Depth;
    Renderpass->Stencil = Stencil;

    // Main subpass
    VkSubpassDescription Subpass = {0};
    Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // Attachments TODO: make this configurable.
    constexpr UInt32 AttachmentDescriptionCount = 2;
    VkAttachmentDescription AttachmentDescriptions[AttachmentDescriptionCount];

    // Color attachment
    VkAttachmentDescription ColorAttachment = {0};
    ColorAttachment.format = Context->Swapchain.ImageFormat.format; // TODO: configurable
    ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    ColorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    ColorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;      // Do not expect any particular layout before render pass starts.
    ColorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;  // Transitioned to after the render pass
    ColorAttachment.flags = 0;

    AttachmentDescriptions[0] = ColorAttachment;

    VkAttachmentReference ColorAttachmentReference;
    ColorAttachmentReference.attachment = 0;  // Attachment description array index
    ColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    Subpass.colorAttachmentCount = 1;
    Subpass.pColorAttachments = &ColorAttachmentReference;

    // Depth attachment, if there is one
    // Might want to make configurable? however it is a 3D engine, maybe for 2D, but againg probably still needed
    VkAttachmentDescription DepthAttachment = {0};
    DepthAttachment.format = Context->Device.DepthFormat;
    DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    DepthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    AttachmentDescriptions[1] = DepthAttachment;

    // Depth attachment reference
    VkAttachmentReference DepthAttachmentReference;
    DepthAttachmentReference.attachment = 1;
    DepthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // TODO: other attachment types (input, resolve, preserve)

    // Depth stencil data.
    Subpass.pDepthStencilAttachment = &DepthAttachmentReference;

    // Input from a shader
    Subpass.inputAttachmentCount = 0;
    Subpass.pInputAttachments = 0;

    // Attachments used for multisampling colour attachments
    Subpass.pResolveAttachments = 0;

    // Attachments not used in this subpass, but must be preserved for the next.
    Subpass.preserveAttachmentCount = 0;
    Subpass.pPreserveAttachments = 0;

    // Render pass dependencies. TODO: make this configurable.
    VkSubpassDependency Dependency = {0};
    Dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    Dependency.dstSubpass = 0;
    Dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    Dependency.srcAccessMask = 0;
    Dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    Dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    Dependency.dependencyFlags = 0;

    // Render pass create.
    VkRenderPassCreateInfo RenderpassCreateInfo = {0};
    RenderpassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    RenderpassCreateInfo.attachmentCount = AttachmentDescriptionCount;
    RenderpassCreateInfo.pAttachments = AttachmentDescriptions;
    RenderpassCreateInfo.subpassCount = 1;
    RenderpassCreateInfo.pSubpasses = &Subpass;
    RenderpassCreateInfo.dependencyCount = 1;
    RenderpassCreateInfo.pDependencies = &Dependency;
    RenderpassCreateInfo.pNext = 0;
    RenderpassCreateInfo.flags = 0;

    VK_CHECK(vkCreateRenderPass(
        Context->Device.LogicalDevice,
        &RenderpassCreateInfo,
        Context->Allocator,
        &Renderpass->Handle));
}

void VulkanRenderpassDestroy(VulkanContext* Context, VulkanRenderpass* Renderpass) {
    if (Renderpass && Renderpass->Handle) {
        vkDestroyRenderPass(Context->Device.LogicalDevice, Renderpass->Handle, Context->Allocator);
        Renderpass->Handle = 0;
    }
}

void VulkanRenderpassBegin(
    VulkanCommandBuffer* CommandBuffer, 
    VulkanRenderpass* Renderpass,
    VkFramebuffer Framebuffer) {

    VkRenderPassBeginInfo BeginInfo = {0};
    BeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    BeginInfo.renderPass = Renderpass->Handle;
    BeginInfo.framebuffer = Framebuffer;
    BeginInfo.renderArea.offset.x = Renderpass->X;
    BeginInfo.renderArea.offset.y = Renderpass->Y;
    BeginInfo.renderArea.extent.width = Renderpass->W;
    BeginInfo.renderArea.extent.height = Renderpass->H;

    VkClearValue ClearValues[2];
    FMZeroMemory(ClearValues, sizeof(VkClearValue) * 2);
    ClearValues[0].color.float32[0] = Renderpass->R;
    ClearValues[0].color.float32[1] = Renderpass->G;
    ClearValues[0].color.float32[2] = Renderpass->B;
    ClearValues[0].color.float32[3] = Renderpass->A;
    ClearValues[1].depthStencil.depth = Renderpass->Depth;
    ClearValues[1].depthStencil.stencil = Renderpass->Stencil;

    BeginInfo.clearValueCount = 2;
    BeginInfo.pClearValues = ClearValues;

    vkCmdBeginRenderPass(CommandBuffer->Handle, &BeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    CommandBuffer->State = VULKAN_COMMAND_BUFFER_STATE_IN_RENDER_PASS;
}

void VulkanRenderpassEnd(VulkanCommandBuffer* CommandBuffer, VulkanRenderpass* Rendepass) {
    (void) Rendepass;    

    vkCmdEndRenderPass(CommandBuffer->Handle);
    CommandBuffer->State = VULKAN_COMMAND_BUFFER_STATE_RECORDING;
}