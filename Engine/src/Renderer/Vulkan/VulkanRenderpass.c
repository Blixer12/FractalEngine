#include "VulkanRenderpass.h"

#include "Core/Memory.h"

void VulkanRenderpassCreate(
    VulkanContext* Context, 
    VulkanRenderpass* Renderpass,
    Vec4 RenderArea,
    Vec4 ClearColor,
    Float32 Depth,
    UInt32 Stencil,
    UInt8 ClearFlags,
    Bool8 HasPreviousPass,
    Bool8 HasNextPass) {
    
    Renderpass->RenderArea = RenderArea;
    Renderpass->ClearColor = ClearColor;

    Renderpass->Depth = Depth;
    Renderpass->Stencil = Stencil;

    Renderpass->ClearFlags = ClearFlags;
    Renderpass->HasPreviousPass = HasPreviousPass;
    Renderpass->HasNextPass = HasNextPass;

    // Main subpass
    VkSubpassDescription Subpass = {0};
    Subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;

    // Attachments TODO: make this configurable.
    UInt32 AttachmentDescriptionCount = 0;
    VkAttachmentDescription AttachmentDescriptions[2];

    // Color attachment
    Bool8 DoClearColor = (Renderpass->ClearFlags & RENDERPASS_CLEAR_COLOR_BUFFER_FLAG) != 0;
    VkAttachmentDescription ColorAttachment = {0};
    ColorAttachment.format = Context->Swapchain.ImageFormat.format; // TODO: configurable
    ColorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    ColorAttachment.loadOp = DoClearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    ColorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    ColorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    ColorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    ColorAttachment.initialLayout = HasPreviousPass ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_UNDEFINED;
    ColorAttachment.finalLayout = HasNextPass ? VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;  // Transitioned to after the render pass
    ColorAttachment.flags = 0;

    AttachmentDescriptions[0] = ColorAttachment;
    AttachmentDescriptionCount++;

    VkAttachmentReference ColorAttachmentReference;
    ColorAttachmentReference.attachment = 0;  // Attachment description array index
    ColorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    Subpass.colorAttachmentCount = 1;
    Subpass.pColorAttachments = &ColorAttachmentReference;

    // Depth attachment, if there is one
    // Might want to make configurable? however it is a 3D engine, maybe for 2D, but againg probably still needed
    Bool8 DoClearDepth = (Renderpass->ClearFlags & RENDERPASS_CLEAR_DEPTH_BUFFER_FLAG) != 0;

    if (DoClearDepth)
    {

    
    VkAttachmentDescription DepthAttachment = {0};
    DepthAttachment.format = Context->Device.DepthFormat;
    DepthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    DepthAttachment.loadOp = DoClearDepth ? VK_ATTACHMENT_LOAD_OP_CLEAR : VK_ATTACHMENT_LOAD_OP_LOAD;
    DepthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    DepthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    DepthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    DepthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    AttachmentDescriptions[AttachmentDescriptionCount] = DepthAttachment;
    AttachmentDescriptionCount++;

    // Depth attachment reference
    VkAttachmentReference DepthAttachmentReference;
    DepthAttachmentReference.attachment = 1;
    DepthAttachmentReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // TODO: other attachment types (input, resolve, preserve)

    // Depth stencil data.
    Subpass.pDepthStencilAttachment = &DepthAttachmentReference;
    } else {
        FMZeroMemory(&AttachmentDescriptions[AttachmentDescriptionCount], sizeof(VkAttachmentDescription));
        Subpass.pDepthStencilAttachment = 0;
    }

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
    BeginInfo.renderArea.offset.x = Renderpass->RenderArea.x;
    BeginInfo.renderArea.offset.y = Renderpass->RenderArea.y;
    BeginInfo.renderArea.extent.width = Renderpass->RenderArea.z;
    BeginInfo.renderArea.extent.height = Renderpass->RenderArea.w;

    BeginInfo.clearValueCount = 0;
    BeginInfo.pClearValues = 0;

    VkClearValue ClearValues[2];
    FMZeroMemory(ClearValues, sizeof(VkClearValue) * 2);
    Bool8 DoClearColor = (Renderpass->ClearFlags & RENDERPASS_CLEAR_COLOR_BUFFER_FLAG) != 0;
    if (DoClearColor)
    {
        FMCopyMemory(ClearValues[BeginInfo.clearValueCount].color.float32, Renderpass->ClearColor.Elements, sizeof(Float32) * 4);\
        BeginInfo.clearValueCount++;
    }

    Bool8 DoClearDepth = (Renderpass->ClearFlags & RENDERPASS_CLEAR_DEPTH_BUFFER_FLAG) != 0;
    if (DoClearDepth)
    {
        FMCopyMemory(ClearValues[BeginInfo.clearValueCount].color.float32, Renderpass->ClearColor.Elements, sizeof(Float32) * 4);
        ClearValues[BeginInfo.clearValueCount].depthStencil.depth = Renderpass->Depth;

        Bool8 DoClearStencil = (Renderpass->ClearFlags & RENDERPASS_CLEAR_STENCIL_BUFFER_FLAG) != 0;
        ClearValues[BeginInfo.clearValueCount].depthStencil.stencil = DoClearStencil ? Renderpass->Stencil : 0;
        BeginInfo.clearValueCount++;
    }

    BeginInfo.pClearValues = BeginInfo.clearValueCount > 0 ? ClearValues : 0;

    vkCmdBeginRenderPass(CommandBuffer->Handle, &BeginInfo, VK_SUBPASS_CONTENTS_INLINE);
    CommandBuffer->State = VULKAN_COMMAND_BUFFER_STATE_IN_RENDER_PASS;
}

void VulkanRenderpassEnd(VulkanCommandBuffer* CommandBuffer, VulkanRenderpass* Rendepass) {
    (void) Rendepass;    

    vkCmdEndRenderPass(CommandBuffer->Handle);
    CommandBuffer->State = VULKAN_COMMAND_BUFFER_STATE_RECORDING;
}