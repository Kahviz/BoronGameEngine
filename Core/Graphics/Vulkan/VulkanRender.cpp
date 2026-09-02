#include "VulkanRender.h"
#include "GLOBALS.h"

#if VULKAN == 1
#include <cstdint>
#include <vulkan/vulkan.h>
#include <iostream>
#include <vector>
#include <thread>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include <string>
#include "ErrorHandling/ErrorMessage.h"
#include "Mesh/Vulkan/MeshVulkan.h"
#include <CameraControl.h>
#include "imgui.h"
#include <imgui_impl_vulkan.h>
#include "Components.h"
#include "Texture.h"
#include "backends/BoronGui_implVulkan.h"
#include <BoronGui.h>

bool VulkanRender::Init(GLFWwindow* window)
{
    float AspectX = (float)screen_width;
    float AspectY = (float)screen_height;
    float Aspect = AspectX / AspectY;

    m_Camera.SetProjectionValues(g_FOV, Aspect, 0.0f, 1000.0f);

    CreateInfo("Vulkan Init Started!");
    viewportTexture = std::make_unique<Texture>();
    if (!vkInstance.Init()) {
        CreateError("A Unexpected error happened on vkInstance.Init");
        return false;
    }

    if (!vkDevice.Init(window, vkInstance.GetInstance())) {
        CreateError("A Unexpected error happened on vkDevice.Init");
        return false;
    }

    if (!vkCommandBuffer.CreateCommandPool(vkDevice.GetDevice(), vkDevice.GetFamilyIndex())) {
        CreateError("A Unexpected error happened on vkDevice.CreateCommandPool");
        return false;
    }

    VkPhysicalDeviceProperties selectedProps;
    vkGetPhysicalDeviceProperties(vkDevice.GetPhysicalDevice(), &selectedProps);

#ifdef _DEBUG
    std::cout << "Selected GPU: " << selectedProps.deviceName << "\n";
#endif // _DEBUG

    uint32_t formatCount = 0;

    vkGetPhysicalDeviceSurfaceFormatsKHR(vkDevice.GetPhysicalDevice(), vkDevice.GetSurface(), &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkDevice.GetPhysicalDevice(), vkDevice.GetSurface(), &formatCount, formats.data());

    VkSurfaceFormatKHR surfaceFormat = formats[0];
    for (const auto& availableFormat : formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = availableFormat;
            break;
        }
    }

    if (!vkSwapchain.Init(vkDevice.GetDevice(), vkDevice.GetPhysicalDevice(), vkDevice.GetSurface())) {
        CreateError("A Unexpected error happened on vkSwapchain.Init");
        return false;
    }

    std::array<VkAttachmentDescription, 2> attachments = {};

    // Color attachment
    attachments[0].format = surfaceFormat.format;
    attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    // Depth attachment
    depthFormat = FindDepthFormat(vkDevice.GetPhysicalDevice());
    attachments[1].format = depthFormat;
    attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Attachment references
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    // Subpass
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    //Create DepthRecources
    CreateDepthResources(vkSwapchain.GetSwapchainExtent().width, vkSwapchain.GetSwapchainExtent().height);

    // Render pass
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    BGE_ASSERT_VKRESULT(vkCreateRenderPass(vkDevice.GetDevice(), &renderPassInfo, nullptr, &renderPass), "Failed to create render pass");
    CreateSuccess("Render pass created");

    depthFormat = FindDepthFormat(vkDevice.GetPhysicalDevice());

    createViewportRenderPass();
    if (!vkPipeline.Init(vkDevice.GetDevice(), viewportRenderPass)) {
        CreateError("A Unexpected error happened on vkPipeline.Init");
    }

    CreateSuccess("Framebuffers created");

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    BGE_ASSERT_VKRESULT(vkCreateSemaphore(vkDevice.GetDevice(), &semaphoreInfo, nullptr, &imageAvailableSemaphore),
        "Failed to create synchronization object");
    BGE_ASSERT_VKRESULT(vkCreateSemaphore(vkDevice.GetDevice(), &semaphoreInfo, nullptr, &renderFinishedSemaphore),
        "Failed to create synchronization object");
    BGE_ASSERT_VKRESULT(vkCreateFence(vkDevice.GetDevice(), &fenceInfo, nullptr, &inFlightFence), "Failed to create fence");

    CreateSuccess("Synchronization objects created");

    uint32_t framebufferCount = static_cast<uint32_t>(vkSwapchain.GetSwapchainFramebuffers().size());

    if (!vkCommandBuffer.AllocateCommandBuffers(vkDevice.GetDevice(), framebufferCount)) {
        CreateError("Failed to allocate command buffers!");
        return false;
    }

    vkSwapchain.GetSwapchainExtent() = vkSwapchain.ChooseSwapchainExtent(vkDevice.GetPhysicalDevice(), vkDevice.GetSurface());
    vkSwapchain.GetSwapchainImageFormat() = surfaceFormat.format;

    VkPhysicalDeviceProperties props{};
    vkGetPhysicalDeviceProperties(vkDevice.GetPhysicalDevice(), &props);

    VkDeviceSize minAlignment = props.limits.minUniformBufferOffsetAlignment;

    VkDeviceSize alignedUBOSize =
        sizeof(UniformBufferObject);

    if (minAlignment > 0) {
        alignedUBOSize =
            (alignedUBOSize + minAlignment - 1) & ~(minAlignment - 1);
    }

    dynamicAlignment = alignedUBOSize;

    VkDescriptorPoolSize pool_sizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo pool_info = {};
    pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
    pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
    pool_info.pPoolSizes = pool_sizes;

    BGE_ASSERT_VKRESULT(vkCreateDescriptorPool(vkDevice.GetDevice(), &pool_info, nullptr, &imguiPool), "Failed to create ImGui descriptor pool");

    createUniformBuffers();

    createShadowRenderPass();
    createShadowResources();
    createShadowPipeline();

    //InitEnd :D-<
    CreateSuccess("No Fatal Errors in Vulkan Initing :D-<");
    return true;
}

VulkanRender::VulkanRender()
{
}

VulkanRender::~VulkanRender()
{
}

void VulkanRender::CreateDepthResources(uint32_t width, uint32_t height) {
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = depthFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    BGE_ASSERT_VKRESULT(vkCreateImage(vkDevice.GetDevice(), &imageInfo, nullptr, &depthImage), "Failed to create depth image!");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(vkDevice.GetDevice(), depthImage, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vkDevice.GetPhysicalDevice()
    );

    BGE_ASSERT_VKRESULT(vkAllocateMemory(vkDevice.GetDevice(), &allocInfo, nullptr, &depthImageMemory), "Failed to allocate depth image memory!");

    vkBindImageMemory(vkDevice.GetDevice(), depthImage, depthImageMemory, 0);

    // Create image view
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = depthImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = depthFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    BGE_ASSERT_VKRESULT(vkCreateImageView(vkDevice.GetDevice(), &viewInfo, nullptr, &depthImageView), "Failed to create depth image view");
}

void VulkanRender::Cleanup()
{
    CreateInfo("Starting Vulkan Cleanup");

    m_UniformBuffer.Destroy();

    if (viewportCommandBuffer != VK_NULL_HANDLE)
    {
        vkFreeCommandBuffers(
            vkDevice.GetDevice(),
            vkCommandBuffer.GetCommandPool(),
            1,
            &viewportCommandBuffer
        );

        viewportCommandBuffer = VK_NULL_HANDLE;
    }

    if (vkDevice.GetDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(vkDevice.GetDevice());
    }

    if (descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(vkDevice.GetDevice(), descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;
    }

    descriptorSets.clear();

    if (imguiPool != VK_NULL_HANDLE) {
        ImGui_ImplVulkan_Shutdown();
        vkDestroyDescriptorPool(vkDevice.GetDevice(), imguiPool, nullptr);
        imguiPool = VK_NULL_HANDLE;
    }

    meshCache.clear();

    if (shadowFramebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(vkDevice.GetDevice(), shadowFramebuffer, nullptr);
        shadowFramebuffer = VK_NULL_HANDLE;
    }
    if (shadowSampler != VK_NULL_HANDLE) {
        vkDestroySampler(vkDevice.GetDevice(), shadowSampler, nullptr);
        shadowSampler = VK_NULL_HANDLE;
    }
    if (shadowImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(vkDevice.GetDevice(), shadowImageView, nullptr);
        shadowImageView = VK_NULL_HANDLE;
    }
    if (shadowImage != VK_NULL_HANDLE) {
        vkDestroyImage(vkDevice.GetDevice(), shadowImage, nullptr);
        shadowImage = VK_NULL_HANDLE;
    }
    if (shadowImageMemory != VK_NULL_HANDLE) {
        vkFreeMemory(vkDevice.GetDevice(), shadowImageMemory, nullptr);
        shadowImageMemory = VK_NULL_HANDLE;
    }
    if (!vkSwapchain.CleanupSwapchain(vkDevice.GetDevice(), vkCommandBuffer.GetCommandPool(), vkCommandBuffer.GetCommandBuffers())) {
        CreateError("A Error happened in vkSwapchain.CleanupSwapchain");
    }
    indexBuffer.Destroy();
    if (indexBufferMemory != VK_NULL_HANDLE) {
        vkFreeMemory(vkDevice.GetDevice(), indexBufferMemory, nullptr);
        indexBufferMemory = VK_NULL_HANDLE;
    }

    if (vkPipeline.GetVertShaderModule() != VK_NULL_HANDLE) {
        vkDestroyShaderModule(vkDevice.GetDevice(), vkPipeline.GetVertShaderModule(), nullptr);
    }
    if (vkPipeline.GetFragShaderModule() != VK_NULL_HANDLE) {
        vkDestroyShaderModule(vkDevice.GetDevice(), vkPipeline.GetFragShaderModule(), nullptr);
    }

    if (depthImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(vkDevice.GetDevice(), depthImageView, nullptr);
        depthImageView = VK_NULL_HANDLE;
    }
    if (depthImage != VK_NULL_HANDLE) {
        vkDestroyImage(vkDevice.GetDevice(), depthImage, nullptr);
        depthImage = VK_NULL_HANDLE;
    }
    if (depthImageMemory != VK_NULL_HANDLE) {
        vkFreeMemory(vkDevice.GetDevice(), depthImageMemory, nullptr);
        depthImageMemory = VK_NULL_HANDLE;
    }

    if (vkDevice.GetSurface() != VK_NULL_HANDLE) {
        vkDestroySurfaceKHR(vkInstance.GetInstance(), vkDevice.GetSurface(), nullptr);
    }

    if (vkDevice.GetDevice() != VK_NULL_HANDLE) {
        vkDestroyDevice(vkDevice.GetDevice(), nullptr);
    }

    if (vkInstance.GetInstance() != VK_NULL_HANDLE) {
        vkDestroyInstance(vkInstance.GetInstance(), nullptr);
    }

    CreateInfo("Cleaned up successfully!");
}

uint32_t VulkanRender::GetImageIndex() {
    return CurrentimageIndex;
}

void VulkanRender::DrawMeshesForRecordCommandBuffer(VkCommandBuffer& cmd) {
    for (const auto& drawCmd : drawCommands)
    {
        bool hasTexture = drawCmd.usesTexture;

        if (hasTexture)
        {
            VkPipeline pipeline = vkPipeline.GetTextureGraphicsPipeline();

            if (pipeline == VK_NULL_HANDLE)
            {
                CreateError("Texture graphics pipeline is VK_NULL_HANDLE!");
                return;
            }

            vkCmdBindPipeline(
                cmd,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                pipeline
            );
        }
        else
        {
            vkCmdBindPipeline(
                cmd,
                VK_PIPELINE_BIND_POINT_GRAPHICS,
                vkPipeline.GetGraphicsPipeline()
            );
        }

        uint32_t dynamicOffset =
            drawCmd.objectIndex * dynamicAlignment;

        VkDescriptorSet descriptorSet =
            descriptorSets[drawCmd.objectIndex];

        vkCmdBindDescriptorSets(
            cmd,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            vkPipeline.GetPipelineLayout(),
            0,
            1,
            &descriptorSet,
            1,
            &dynamicOffset
        );

        if (drawCmd.mesh) {
            drawCmd.mesh->Draw(cmd);
        }
    }
}

void VulkanRender::RecordCommandBuffer(uint32_t imageIndex, bool renderImGui, bool usesTexture) {
    CurrentimageIndex = imageIndex;
    VkCommandBuffer cmd = vkCommandBuffer.GetCommandBuffers()[imageIndex];
    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo beginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
    vkBeginCommandBuffer(cmd, &beginInfo);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)screen_width;
    viewport.height = (float)screen_height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = { (uint32_t)screen_width, (uint32_t)screen_height };
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    VkRenderPassBeginInfo rp{ VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO };
    rp.renderPass = renderPass;
    rp.framebuffer = vkSwapchain.GetSwapchainFramebuffers()[imageIndex];
    rp.renderArea.extent = vkSwapchain.GetSwapchainExtent();
    rp.clearValueCount = static_cast<uint32_t>(clearValues.size());
    rp.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(cmd, &rp, VK_SUBPASS_CONTENTS_INLINE);

#if INEDITOR == 0
    CreateSuccess("Not editing...");
    vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, vkPipeline.GetGraphicsPipeline());

    DrawMeshesForRecordCommandBuffer(cmd);
#endif

    if (renderImGui && ImGui::GetDrawData()) {
        ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);
    }

    PerFrameStuct perFrameStruct{};
    perFrameStruct.commandBuffer = cmd;

    BoronGui::UpdatePerFrameOBJ(perFrameStruct);
    BoronGui_implVulkan::SetupRenderState(cmd);

    BoronGui::DrawWidgets();

    //Render here
    vkCmdEndRenderPass(cmd);
    vkEndCommandBuffer(cmd);
}

void VulkanRender::RecreateSwapchain() {
    vkDeviceWaitIdle(vkDevice.GetDevice());

    vkSwapchain.CleanupSwapchain(vkDevice.GetDevice(), vkCommandBuffer.GetCommandPool(), vkCommandBuffer.GetCommandBuffers());

    CreateSwapchain();
    CreateImageViews();
    CreateDepthResources(vkSwapchain.GetSwapchainExtent().width, vkSwapchain.GetSwapchainExtent().height);
    CreateFramebuffers();
    uint32_t framebufferCount = static_cast<uint32_t>(vkSwapchain.GetSwapchainFramebuffers().size());

    if (!vkCommandBuffer.AllocateCommandBuffers(vkDevice.GetDevice(), framebufferCount)) {
        CreateError("Failed to allocate command buffers at RecreateSwapchain!");
    }

    for (size_t i = 0; i < vkCommandBuffer.GetCommandBuffers().size(); i++) {
        RecordCommandBuffer(static_cast<uint32_t>(i), false, false);
    }
}

void VulkanRender::CreateFramebuffers() {
    vkSwapchain.GetSwapchainFramebuffers().resize(vkSwapchain.GetSwapchainImageViews().size());

    for (size_t i = 0; i < vkSwapchain.GetSwapchainImageViews().size(); i++) {
        std::array<VkImageView, 2> attachments = {
            vkSwapchain.GetSwapchainImageViews()[i],
            depthImageView
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = vkSwapchain.GetSwapchainExtent().width;
        framebufferInfo.height = vkSwapchain.GetSwapchainExtent().height;
        framebufferInfo.layers = 1;
        BGE_ASSERT_VKRESULT(vkCreateFramebuffer(vkDevice.GetDevice(), &framebufferInfo, nullptr, &vkSwapchain.GetSwapchainFramebuffers()[i]),
            "Failed to create framebuffer");
    }
}

void VulkanRender::CreateImageViews() {
    vkSwapchain.GetSwapchainImageViews().resize(vkSwapchain.GetSwapchainImages().size());

    for (size_t i = 0; i < vkSwapchain.GetSwapchainImages().size(); i++) {
        VkImageViewCreateInfo viewInfo{};
        viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewInfo.image = vkSwapchain.GetSwapchainImages()[i];
        viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewInfo.format = vkSwapchain.GetSwapchainImageFormat();
        viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
        viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewInfo.subresourceRange.baseMipLevel = 0;
        viewInfo.subresourceRange.levelCount = 1;
        viewInfo.subresourceRange.baseArrayLayer = 0;
        viewInfo.subresourceRange.layerCount = 1;

        BGE_ASSERT_VKRESULT(vkCreateImageView(vkDevice.GetDevice(), &viewInfo, nullptr, &vkSwapchain.GetSwapchainImageViews()[i]),
            "Failed to create image views");
    }
}

void VulkanRender::CreateSwapchain() {
    VkSurfaceCapabilitiesKHR surfaceCapabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vkDevice.GetPhysicalDevice(), vkDevice.GetSurface(), &surfaceCapabilities);

    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkDevice.GetPhysicalDevice(), vkDevice.GetSurface(), &formatCount, nullptr);
    std::vector<VkSurfaceFormatKHR> formats(formatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(vkDevice.GetPhysicalDevice(), vkDevice.GetSurface(), &formatCount, formats.data());

    VkSurfaceFormatKHR surfaceFormat = formats[0];
    for (const auto& availableFormat : formats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            surfaceFormat = availableFormat;
            break;
        }
    }

    VkPresentModeKHR presentMode;
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(vkDevice.GetPhysicalDevice(), vkDevice.GetSurface(), &presentModeCount, nullptr);
    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(vkDevice.GetPhysicalDevice(), vkDevice.GetSurface(), &presentModeCount, presentModes.data());

    if (g_vSync) {
        presentMode = VK_PRESENT_MODE_FIFO_KHR;
    }
    else {
        presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;

        for (const auto& mode : presentModes) {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
                presentMode = mode;
                break;
            }
        }
    }

    VkExtent2D extent = surfaceCapabilities.currentExtent;
    if (extent.width == UINT32_MAX || extent.width == 0) {
        // width
        uint32_t minWidth = surfaceCapabilities.minImageExtent.width;
        uint32_t maxWidth = surfaceCapabilities.maxImageExtent.width;
        uint32_t targetWidth = static_cast<uint32_t>(screen_width);

        if (targetWidth < minWidth)
            extent.width = minWidth;
        else if (targetWidth > maxWidth)
            extent.width = maxWidth;
        else
            extent.width = targetWidth;

        uint32_t minHeight = surfaceCapabilities.minImageExtent.height;
        uint32_t maxHeight = surfaceCapabilities.maxImageExtent.height;
        uint32_t targetHeight = static_cast<uint32_t>(screen_height);

        if (targetHeight < minHeight)
            extent.height = minHeight;
        else if (targetHeight > maxHeight)
            extent.height = maxHeight;
        else
            extent.height = targetHeight;
    }

    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount) {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = vkDevice.GetSurface();
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = extent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;
    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    BGE_ASSERT_VKRESULT(vkCreateSwapchainKHR(vkDevice.GetDevice(), &swapchainCreateInfo, nullptr, &vkSwapchain.GetSwapchain()), "Failed to create swapchain");

    vkSwapchain.GetSwapchainExtent() = extent;
    vkSwapchain.GetSwapchainImageFormat() = surfaceFormat.format;

    uint32_t swapchainImageCount = 0;
    vkGetSwapchainImagesKHR(vkDevice.GetDevice(), vkSwapchain.GetSwapchain(), &swapchainImageCount, nullptr);
    vkSwapchain.GetSwapchainImages().resize(swapchainImageCount);
    vkGetSwapchainImagesKHR(vkDevice.GetDevice(), vkSwapchain.GetSwapchain(), &swapchainImageCount, vkSwapchain.GetSwapchainImages().data());
}

void VulkanRender::createUniformBuffers() {
    m_CurrentObjectCount = 500;
    ReallocateUniformBuffer(m_CurrentObjectCount);
}

void VulkanRender::ReallocateUniformBuffer(uint32_t newObjectCount, ECS* ecs) {
    vkDeviceWaitIdle(vkDevice.GetDevice());

    if (ecs != nullptr) {
        newObjectCount = ecs->getNumberOfEntities() + 1000;
    }
    m_UniformBufferSize = dynamicAlignment * newObjectCount;
    m_CurrentObjectCount = newObjectCount;

    if (m_UniformBuffer.GetBuffer() == VK_NULL_HANDLE) {
        m_UniformBuffer.Create(
            vkDevice.GetDevice(),
            vkDevice.GetPhysicalDevice(),
            m_UniformBufferSize,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        m_UniformBuffer.Map();
    }
    else {
        m_UniformBuffer.Resize(m_UniformBufferSize, vkCommandBuffer.GetCommandPool(), vkDevice.GetGraphicsQueue());
    }
}

void VulkanRender::createDescriptorPool(uint32_t maxObjects)
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSizes[0].descriptorCount = maxObjects;

    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = maxObjects * 2;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = maxObjects;

    BGE_ASSERT_VKRESULT(
        vkCreateDescriptorPool(
            vkDevice.GetDevice(),
            &poolInfo,
            nullptr,
            &descriptorPool
        ),
        "Failed to create descriptor pool"
    );
}

void VulkanRender::UpdateDescriptorSets(ECS& ecs)
{
    vkDeviceWaitIdle(vkDevice.GetDevice());

    if (descriptorPool != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorPool(vkDevice.GetDevice(), descriptorPool, nullptr);
        descriptorPool = VK_NULL_HANDLE;
    }

    descriptorSets.clear();

    uint32_t maxEntityId = 0;
    bool hasEntities = false;
    ecs.EachEntity([&](EntityECS entity) {
        maxEntityId = std::max(maxEntityId, entity);
        hasEntities = true;
        });

    if (!hasEntities)
        return;

    uint32_t count = maxEntityId + 1;

    createDescriptorPool(count);
    createDescriptorSets(ecs, count);
}
void VulkanRender::createDescriptorSets(ECS& ecs, uint32_t count)
{
    descriptorSets.resize(count);

    std::vector<VkDescriptorSetLayout> layouts(
        count,
        vkPipeline.GetDescriptorSetLayout()
    );

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = descriptorPool;
    allocInfo.descriptorSetCount = count;
    allocInfo.pSetLayouts = layouts.data();

    BGE_ASSERT_VKRESULT(
        vkAllocateDescriptorSets(
            vkDevice.GetDevice(),
            &allocInfo,
            descriptorSets.data()
        ),
        "Failed to allocate descriptor sets"
    );

    ecs.Each<ObjectComponent>(
        [&](EntityECS entity, ObjectComponent& obj)
        {
            const Texture* texture = nullptr;

            if (ecs.HasComponent<TextureComponent>(entity))
            {
                texture = ecs.GetComponent<TextureComponent>(entity).texture;
            }

            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = m_UniformBuffer.GetBuffer();
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UniformBufferObject);

            VkDescriptorImageInfo shadowImageInfo{};
            shadowImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            shadowImageInfo.imageView = shadowImageView;
            shadowImageInfo.sampler = shadowSampler;

            std::array<VkWriteDescriptorSet, 3> writes{};
            writes[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[0].dstSet = descriptorSets[entity];
            writes[0].dstBinding = 0;
            writes[0].descriptorCount = 1;
            writes[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            writes[0].pBufferInfo = &bufferInfo;

            writes[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writes[2].dstSet = descriptorSets[entity];
            writes[2].dstBinding = 2;
            writes[2].descriptorCount = 1;
            writes[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writes[2].pImageInfo = &shadowImageInfo;

            if (texture != nullptr && texture->IsLoadedConst())
            {
                VkDescriptorImageInfo imageInfo{};
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = texture->GetImageView();
                imageInfo.sampler = texture->GetSampler();

                writes[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                writes[1].dstSet = descriptorSets[entity];
                writes[1].dstBinding = 1;
                writes[1].descriptorCount = 1;
                writes[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                writes[1].pImageInfo = &imageInfo;

                vkUpdateDescriptorSets(vkDevice.GetDevice(), 3, writes.data(), 0, nullptr);
            }
            else
            {
                VkWriteDescriptorSet texturelessWrites[2] = { writes[0], writes[2] };
                vkUpdateDescriptorSets(vkDevice.GetDevice(), 2, texturelessWrites, 0, nullptr);
            }
        }
    );
}

BML::Matrix4x4 VulkanRender::CreateVulkanPerspective(float fovY, float aspect, float zNear, float zFar) {
    BML::Matrix4x4 result(0.0f);
    float f = 1.0f / tanf(fovY * 0.5f);
    float rangeInv = 1.0f / (zFar - zNear);

    result(0, 0) = f / aspect;
    result(1, 1) = -f;
    result(2, 2) = zFar * rangeInv;
    result(2, 3) = 1.0f;
    result(3, 2) = -(zFar * zNear) * rangeInv;

    return result;
}

BML::Matrix4x4 VulkanRender::createModelMatrix(BML::Vector3 orientation, BML::Vector3 scale, BML::Vector3 pos) { //Add to mathlib
    BML::Matrix4x4 model;

    float cx = cosf(orientation.x());
    float sx = sinf(orientation.x());
    float cy = cosf(orientation.y());
    float sy = sinf(orientation.y());
    float cz = cosf(orientation.z());
    float sz = sinf(orientation.z());

    BML::Matrix4x4 rot;

    rot(0, 0) = cy * cz;
    rot(0, 1) = sx * sy * cz - cx * sz;
    rot(0, 2) = cx * sy * cz + sx * sz;
    rot(0, 3) = 0.0f;

    rot(1, 0) = cy * sz;
    rot(1, 1) = sx * sy * sz + cx * cz;
    rot(1, 2) = cx * sy * sz - sx * cz;
    rot(1, 3) = 0.0f;

    rot(2, 0) = -sy;
    rot(2, 1) = sx * cy;
    rot(2, 2) = cx * cy;
    rot(2, 3) = 0.0f;

    rot(3, 0) = 0.0f;
    rot(3, 1) = 0.0f;
    rot(3, 2) = 0.0f;
    rot(3, 3) = 1.0f;

    model.setIdentity();

    //Scale
    model(0, 0) = rot(0, 0) * scale.x();
    model(0, 1) = rot(0, 1) * scale.x();
    model(0, 2) = rot(0, 2) * scale.x();

    model(1, 0) = rot(1, 0) * scale.y();
    model(1, 1) = rot(1, 1) * scale.y();
    model(1, 2) = rot(1, 2) * scale.y();

    model(2, 0) = rot(2, 0) * scale.z();
    model(2, 1) = rot(2, 1) * scale.z();
    model(2, 2) = rot(2, 2) * scale.z();

    //Translaation
    model(3, 0) = pos.x();
    model(3, 1) = pos.y();
    model(3, 2) = pos.z();
    model(3, 3) = 1.0f;

    return model;
}

void VulkanRender::updateUniformBuffer(
    ECS& ecs,
    EntityECS entity,
    uint32_t objectIndex,
    BML::Vector3 scale,
    BML::Vector3 Orientation,
    BML::Vector3 pos,
    BML::Int3 color
)
{
    if (objectIndex >= m_CurrentObjectCount)
    {
        uint32_t newSize = std::max(m_CurrentObjectCount * 2, objectIndex + 1);
        ReallocateUniformBuffer(newSize, &ecs);
    }

    UniformBufferObject ubo{};

    ubo.model = createModelMatrix(Orientation, scale, pos);

    ubo.color = GPUVector3(
        color.x() / 255.0f,
        color.y() / 255.0f,
        color.z() / 255.0f
    );
    ubo.UsesTexture = false;
    if (ecs.HasComponent<TextureComponent>(entity))
    {
        const Texture* tex =
            ecs.GetComponent<TextureComponent>(entity).texture;
        ubo.UsesTexture = true;
    }

    ubo.view = m_Camera.GetViewMatrix().transposed();
    float aspect = -1.0f;

    #if INEDITOR == 1
        aspect = (float)viewport_width / (float)viewport_height;
    #endif
    #if INEDITOR == 0
        aspect = (float)screen_width / (float)screen_height;
    #endif
    ubo.proj = CreateVulkanPerspective(
        45.0f * PI / 180.0f,
        aspect,
        0.1f,
        zFar
    );

    ubo.lightSpaceMatrix = lightSpaceMatrix;

    uint8_t* dst = (uint8_t*)m_UniformBuffer.GetMappedMemory() + (objectIndex * dynamicAlignment);
    
    memcpy(dst, &ubo, sizeof(ubo));
}

bool VulkanRender::RenderAMesh(ECS& ecs, EntityECS entity)
{
    if (!ecs.HasComponent<TransformComponent>(entity) ||
        !ecs.HasComponent<ColorComponent>(entity) ||
        !ecs.HasComponent<ObjectComponent>(entity))
    {
        CreateError("Missing rendering component");
        return false;
    }
    auto& transform = ecs.GetComponent<TransformComponent>(entity);
    auto& color = ecs.GetComponent<ColorComponent>(entity);
    auto& mesh = ecs.GetComponent<ObjectComponent>(entity);

    updateUniformBuffer(
        ecs,
        entity,
        entity,
        transform.transform.Size,
        transform.transform.Orientation,
        transform.transform.Position,
        color.color
    );

    DrawCommand cmd{};
    if (!mesh.OBJmesh) {
        CreateError("OBJmesh is NULL");
        return false;
    }
    cmd.mesh = &mesh.OBJmesh->VM;
    cmd.objectIndex = entity;
    cmd.usesTexture = ecs.HasComponent<TextureComponent>(entity)
        && ecs.GetComponent<TextureComponent>(entity).texture != nullptr
        && ecs.GetComponent<TextureComponent>(entity).texture->IsLoaded();

    cmd.modelMatrix = createModelMatrix(
        transform.transform.Orientation,
        transform.transform.Size,
        transform.transform.Position
    );

    drawCommands.push_back(cmd);

    return true;
}
void VulkanRender::resizeViewport(uint32_t width, uint32_t height) {
    if (width == 0 || height == 0)
        return;

    vkDeviceWaitIdle(vkDevice.GetDevice());

    if (viewportFramebuffer != VK_NULL_HANDLE)
    {
        vkDestroyFramebuffer(
            vkDevice.GetDevice(),
            viewportFramebuffer,
            nullptr
        );

        viewportFramebuffer = VK_NULL_HANDLE;
    }

    if (viewportDepthView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(
            vkDevice.GetDevice(),
            viewportDepthView,
            nullptr
        );

        viewportDepthView = VK_NULL_HANDLE;
    }

    if (viewportDepthImage != VK_NULL_HANDLE)
    {
        vkDestroyImage(
            vkDevice.GetDevice(),
            viewportDepthImage,
            nullptr
        );

        viewportDepthImage = VK_NULL_HANDLE;
    }

    if (viewportDepthMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(
            vkDevice.GetDevice(),
            viewportDepthMemory,
            nullptr
        );

        viewportDepthMemory = VK_NULL_HANDLE;
    }

    viewportTexture->DestroyRenderTarget(
        vkDevice.GetDevice()
    );

    viewportTexture->CreateRenderTarget(
        vkDevice.GetDevice(),
        vkDevice.GetPhysicalDevice(),
        width,
        height,
        viewportRenderPass
    );

    createViewportDepthResources(width, height);

    VkImageView attachments[] =
    {
        viewportTexture->GetImageView(),
        viewportDepthView
    };

    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType =
        VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;

    framebufferInfo.renderPass = viewportRenderPass;
    framebufferInfo.attachmentCount = 2;
    framebufferInfo.pAttachments = attachments;
    framebufferInfo.width = width;
    framebufferInfo.height = height;
    framebufferInfo.layers = 1;

    BGE_ASSERT_VKRESULT(
        vkCreateFramebuffer(
            vkDevice.GetDevice(),
            &framebufferInfo,
            nullptr,
            &viewportFramebuffer
        ),
        "Failed to recreate viewport framebuffer"
    );

    ImTextureID imGuiViewportDescriptor =
        (ImTextureID)ImGui_ImplVulkan_AddTexture(
            viewportTexture->GetSampler(),
            viewportTexture->GetImageView(),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );

    viewportTexture->SetImGuiTexture(imGuiViewportDescriptor);

    viewport_width = width;
    viewport_height = height;
}

void VulkanRender::PrintInfo() {
    if (shadowImage == VK_NULL_HANDLE) {
        CreateError("shadowImage is VK_NULL_HANDLE");
    }
    if (shadowImageView == VK_NULL_HANDLE) {
        CreateError("shadowImageView is VK_NULL_HANDLE");
    }
    if (shadowImageMemory == VK_NULL_HANDLE) {
        CreateError("shadowImageMemory is VK_NULL_HANDLE");
    }
    if (shadowSampler == VK_NULL_HANDLE) {
        CreateError("shadowSampler is VK_NULL_HANDLE");
    }
    if (shadowRenderPass == VK_NULL_HANDLE) {
        CreateError("shadowRenderPass is VK_NULL_HANDLE");
    }
    if (shadowPipeline == VK_NULL_HANDLE) {
        CreateError("shadowPipeline is VK_NULL_HANDLE");
    }
    if (shadowPipelineLayout == VK_NULL_HANDLE) {
        CreateError("shadowPipelineLayout is VK_NULL_HANDLE");
    }
    if (shadowFramebuffer == VK_NULL_HANDLE) {
        CreateError("shadowFramebuffer is VK_NULL_HANDLE");
    }
    if (shadowCommandBuffer == VK_NULL_HANDLE) {
        CreateError("shadowCommandBuffer is VK_NULL_HANDLE");
    }
    CreateInfo("Checked all the shadowResources");
}

void VulkanRender::RecordShadowCommandBuffer()
{
    if (shadowDrawCommands.empty()) {
        return;
    }

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    BGE_ASSERT_VKRESULT(vkBeginCommandBuffer(shadowCommandBuffer, &beginInfo), "Failed to begin shadow command buffer");

    VkClearValue clearValue{};
    clearValue.depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo rpInfo{};
    rpInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rpInfo.renderPass = shadowRenderPass;
    rpInfo.framebuffer = shadowFramebuffer;
    rpInfo.renderArea.extent = { SHADOW_MAP_SIZE, SHADOW_MAP_SIZE };
    rpInfo.clearValueCount = 1;
    rpInfo.pClearValues = &clearValue;

    vkCmdBeginRenderPass(shadowCommandBuffer, &rpInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(shadowCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, shadowPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)SHADOW_MAP_SIZE;
    viewport.height = (float)SHADOW_MAP_SIZE;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(shadowCommandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = { SHADOW_MAP_SIZE, SHADOW_MAP_SIZE };
    vkCmdSetScissor(shadowCommandBuffer, 0, 1, &scissor);

    for (const auto& cmd : shadowDrawCommands)
    {
        ShadowPushConstants pc{};
        pc.lightSpaceMatrix = lightSpaceMatrix;
        pc.model = cmd.modelMatrix;

        vkCmdPushConstants(
            shadowCommandBuffer,
            shadowPipelineLayout,
            VK_SHADER_STAGE_VERTEX_BIT,
            0, sizeof(ShadowPushConstants), &pc
        );

        cmd.mesh->Draw(shadowCommandBuffer);
    }

    vkCmdEndRenderPass(shadowCommandBuffer);

    BGE_ASSERT_VKRESULT(vkEndCommandBuffer(shadowCommandBuffer), "Failed to end shadow command buffer");
}

inline BML::Matrix4x4 CreateOrthographic(
    float left, float right,
    float bottom, float top,
    float zNear, float zFar)
{
    BML::Matrix4x4 result(0.0f);
    result(0, 0) = 2.0f / (right - left);
    result(1, 1) = 2.0f / (bottom - top); //Flipped y
    result(2, 2) = 1.0f / (zFar - zNear);
    result(3, 0) = -(right + left) / (right - left);
    result(3, 1) = -(bottom + top) / (bottom - top);
    result(3, 2) = -zNear / (zFar - zNear);
    result(3, 3) = 1.0f;
    return result;
}

void VulkanRender::ClearBuffer(float r, float b, float g) {
    clearValues[0].color = { {r,b,g} };
    clearValues[1].depthStencil = { 1.0f, 0 };
}

void VulkanRender::DrawFrame(ECS& ecs,float deltaTime)
{
    uint32_t maxObjectIndex = 0;
    bool hasEntities = false;

    ecs.EachEntity(
        [&](EntityECS entity)
        {
            maxObjectIndex = std::max(maxObjectIndex, entity);
            hasEntities = true;
        }
    );

    if (hasEntities && maxObjectIndex >= m_CurrentObjectCount)
    {
        const uint32_t newSize =
            Max(m_CurrentObjectCount * 2, maxObjectIndex + 1);

        ReallocateUniformBuffer(newSize,&ecs);
    }

    static uint32_t lastFrameSize = 0;

    uint32_t currentSize = ecs.getNumberOfEntities();

    if (currentSize != lastFrameSize)
    {
        descriptorsDirty = true;
        lastFrameSize = currentSize;
    }

    if (descriptorsDirty)
    {
        UpdateDescriptorSets(ecs);
        descriptorsDirty = false;
    }

    VkResult result = vkAcquireNextImageKHR(
        vkDevice.GetDevice(), vkSwapchain.GetSwapchain(), UINT64_MAX,
        imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        drawCommands.clear();
        shadowDrawCommands.clear();
        RecreateSwapchain();
        return;
    }

    BGE_ASSERT_VKRESULT(result, "Failed to acquire swapchain image");

    BML::Vector3 lightDir = BML::Vector3(0.5f, -1.0f, 0.5f).normalized();
    BML::Vector3 center = BML::Vector3(0, 0, 0);
    BML::Matrix4x4 lightView = Matrix4x4LookAtLH(
        center - lightDir * 30.0f,
        center,
        BML::Vector3(0, 1, 0)
    );
    float OrthoSize = 20.0f;
    BML::Matrix4x4 lightProj = CreateOrthographic(-OrthoSize, OrthoSize, -OrthoSize, OrthoSize, 0.1f, 80.0f);
    lightSpaceMatrix = lightProj * lightView;

    shadowDrawCommands.clear();

    for (const auto& cmd : drawCommands) {
        ShadowDrawCommand shadowCmd{};
        shadowCmd.mesh = cmd.mesh;
        shadowCmd.modelMatrix = cmd.modelMatrix;
        shadowDrawCommands.push_back(shadowCmd);
    }

    RecordShadowCommandBuffer();

    if (!shadowDrawCommands.empty()) {
        VkSubmitInfo shadowSubmitInfo{};
        shadowSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        shadowSubmitInfo.commandBufferCount = 1;
        shadowSubmitInfo.pCommandBuffers = &shadowCommandBuffer;
        BGE_ASSERT_VKRESULT(vkQueueSubmit(vkDevice.GetGraphicsQueue(), 1, &shadowSubmitInfo, VK_NULL_HANDLE), "Failed to submit shadow");
        BGE_ASSERT_VKRESULT(vkQueueWaitIdle(vkDevice.GetGraphicsQueue()), "Failed to wait");

        VkCommandBuffer transitionCmd = BeginSingleTimeCommands();

        VkImageMemoryBarrier barrier{};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.image = shadowImage;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        vkCmdPipelineBarrier(
            transitionCmd,
            VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        EndSingleTimeCommands(transitionCmd);
    }

    if (framebufferResized) {
        framebufferResized = false;
        drawCommands.clear();
        shadowDrawCommands.clear();
        RecreateSwapchain();
        return;
    }

    #if INEDITOR == 1
        RecordViewportCommandBuffer();

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &viewportCommandBuffer;

        BGE_ASSERT_VKRESULT(
            vkQueueSubmit(
                vkDevice.GetGraphicsQueue(),
                1,
                &submitInfo,
                VK_NULL_HANDLE
            ),
            "Failed to submit viewport command buffer"
        );

        vkQueueWaitIdle(vkDevice.GetGraphicsQueue());
    #endif

    bool RenderImGui = true;
    RecordCommandBuffer(imageIndex, RenderImGui, false);
}

void VulkanRender::EndFrame() {
    vkWaitForFences(vkDevice.GetDevice(), 1, &inFlightFence, VK_TRUE, UINT64_MAX);
    vkResetFences(vkDevice.GetDevice(), 1, &inFlightFence);

    VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
    VkSemaphore waitSemaphores[] = { imageAvailableSemaphore };
    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &vkCommandBuffer.GetCommandBuffers()[imageIndex];

    VkSemaphore signalSemaphores[] = { renderFinishedSemaphore };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    BGE_ASSERT_VKRESULT(vkQueueSubmit(vkDevice.GetGraphicsQueue(), 1, &submitInfo, inFlightFence), "Failed to submit draw command buffer");

    VkPresentInfoKHR presentInfo{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &vkSwapchain.GetSwapchain();
    presentInfo.pImageIndices = &imageIndex;

    VkResult result = vkQueuePresentKHR(vkDevice.GetGraphicsQueue(), &presentInfo);

    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        drawCommands.clear();
        shadowDrawCommands.clear();
        RecreateSwapchain();
    }
    BGE_ASSERT_VKRESULT(result, "Failed to present swapchain image");

    drawCommands.clear();
    shadowDrawCommands.clear();
}

Camera& VulkanRender::GetCamera()
{
    return m_Camera;
}

VkCommandBuffer VulkanRender::BeginSingleTimeCommands() {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = vkCommandBuffer.GetCommandPool();
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    BGE_ASSERT_VKRESULT(vkAllocateCommandBuffers(vkDevice.GetDevice(), &allocInfo, &commandBuffer), "Failed to allocate command buffer for single time commands");

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    BGE_ASSERT_VKRESULT(vkBeginCommandBuffer(commandBuffer, &beginInfo), "Failed to begin command buffer");
    return commandBuffer;
}

void VulkanRender::EndSingleTimeCommands(VkCommandBuffer commandBuffer) {
    if (commandBuffer == VK_NULL_HANDLE) return;

    BGE_ASSERT_VKRESULT(vkEndCommandBuffer(commandBuffer), "Failed to end command buffer");

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    BGE_ASSERT_VKRESULT(vkQueueSubmit(vkDevice.GetGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE), "Failed to submit command buffer");

    vkQueueWaitIdle(vkDevice.GetGraphicsQueue());

    vkFreeCommandBuffers(vkDevice.GetDevice(), vkCommandBuffer.GetCommandPool(), 1, &commandBuffer);
}

//Shadows
void VulkanRender::createShadowResources()
{
    BGE_VK_ASSERT(vkDevice.GetPhysicalDevice(), "Physical device is VK_NULL_HANDLE!");

    VkFormatProperties formatProps;
    vkGetPhysicalDeviceFormatProperties(vkDevice.GetPhysicalDevice(), VK_FORMAT_D32_SFLOAT, &formatProps);

    if (!(formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)) {
        CreateError("VK_FORMAT_D32_SFLOAT does not support depth attachment!");
        return;
    }

    VkImageCreateInfo imageinfo{};
    imageinfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageinfo.imageType = VK_IMAGE_TYPE_2D;
    imageinfo.extent.width = SHADOW_MAP_SIZE;
    imageinfo.extent.height = SHADOW_MAP_SIZE;
    imageinfo.extent.depth = 1;
    imageinfo.mipLevels = 1;
    imageinfo.arrayLayers = 1;
    imageinfo.format = VK_FORMAT_D32_SFLOAT;
    imageinfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageinfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageinfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageinfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageinfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    BGE_ASSERT_VKRESULT(vkCreateImage(vkDevice.GetDevice(), &imageinfo, nullptr, &shadowImage), "Failed to create shadowImage");

    if (shadowImage == VK_NULL_HANDLE) {
        CreateError("shadowImage is NULL even though creation succeeded!");
        return;
    }

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(vkDevice.GetDevice(), shadowImage, &memRequirements);

    if (memRequirements.size == 0) {
        CreateError("Memory size is 0");
        CreateError("This suggests a driver bug or invalid image parameters");
        return;
    }

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(
        memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vkDevice.GetPhysicalDevice()
    );

    BGE_ASSERT_VKRESULT(vkAllocateMemory(vkDevice.GetDevice(), &allocInfo, nullptr, &shadowImageMemory), "Failed to allocate memory for shadowImageMemory");
    BGE_ASSERT_VKRESULT(vkBindImageMemory(vkDevice.GetDevice(), shadowImage, shadowImageMemory, 0), "Failed to bind memory for shadowImageMemory");

    CreateSuccess("Allocated shadowImageMemory");

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = shadowImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_D32_SFLOAT;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    BGE_ASSERT_VKRESULT(vkCreateImageView(vkDevice.GetDevice(), &viewInfo, nullptr, &shadowImageView), "Failed to create shadowImageView");

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.compareEnable = VK_TRUE;
    samplerInfo.compareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

    BGE_ASSERT_VKRESULT(vkCreateSampler(vkDevice.GetDevice(), &samplerInfo, nullptr, &shadowSampler), "Failed to create shadow sampler");
    VkFramebufferCreateInfo framebufferInfo{};
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass = shadowRenderPass;
    framebufferInfo.attachmentCount = 1;
    framebufferInfo.pAttachments = &shadowImageView;
    framebufferInfo.width = SHADOW_MAP_SIZE;
    framebufferInfo.height = SHADOW_MAP_SIZE;
    framebufferInfo.layers = 1;

    BGE_ASSERT_VKRESULT(vkCreateFramebuffer(vkDevice.GetDevice(), &framebufferInfo, nullptr, &shadowFramebuffer), "Failed to create shadow framebuffer");

    VkCommandBufferAllocateInfo commandAllocInfo{};
    commandAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandAllocInfo.commandPool = vkCommandBuffer.GetCommandPool();
    commandAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandAllocInfo.commandBufferCount = 1;

    BGE_ASSERT_VKRESULT(vkAllocateCommandBuffers(vkDevice.GetDevice(), &commandAllocInfo, &shadowCommandBuffer), "Failed to allocate ShadowCommandBuffer");

    CreateSuccess("Shadow resources created successfully!");
}

void VulkanRender::createShadowRenderPass() {
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = VK_FORMAT_D32_SFLOAT;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 0;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 0;
    subpass.pColorAttachments = nullptr;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
    dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &depthAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    BGE_ASSERT_VKRESULT(vkCreateRenderPass(vkDevice.GetDevice(), &renderPassInfo, nullptr, &shadowRenderPass), "Failed to create shadowRenderpass");
}

void VulkanRender::createShadowPipeline() {
    std::string Shaders = std::filesystem::current_path().string() + "\\";
    std::filesystem::path fullpath = Shaders;

    if (!fs::exists(Shaders + "\\vertex.spv")) {
        CreateInfo("Shaders searching... in VulkanRender.cpp");

        Shaders = std::string(PROJECT_DIR) + "Core\\Shaders\\";
    }
    auto vertShaderCode = ReadFile(Shaders + "shadow_vertex.spv");
    VkShaderModule vertShaderModule = CreateShaderModule(vkDevice.GetDevice(), vertShaderCode);

    VkPipelineShaderStageCreateInfo vertStage{};
    vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStage.module = vertShaderModule;
    vertStage.pName = "main";

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(ShadowPushConstants);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    BGE_ASSERT_VKRESULT(vkCreatePipelineLayout(vkDevice.GetDevice(), &pipelineLayoutInfo, nullptr, &shadowPipelineLayout),
        "Failed to create shadow pipeline layout!");

    VkVertexInputBindingDescription binding{};
    binding.binding = 0;
    binding.stride = sizeof(Vertex);
    binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    VkVertexInputAttributeDescription attribute{};
    attribute.binding = 0;
    attribute.location = 0;
    attribute.format = VK_FORMAT_R32G32B32_SFLOAT;
    attribute.offset = offsetof(Vertex, pos);

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &binding;
    vertexInputInfo.vertexAttributeDescriptionCount = 1;
    vertexInputInfo.pVertexAttributeDescriptions = &attribute;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)SHADOW_MAP_SIZE;
    viewport.height = (float)SHADOW_MAP_SIZE;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = { SHADOW_MAP_SIZE, SHADOW_MAP_SIZE };

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_TRUE;
    rasterizer.depthBiasConstantFactor = 1.15f;
    rasterizer.depthBiasSlopeFactor = 1.5f;
    rasterizer.depthBiasClamp = 0.0f;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.attachmentCount = 0;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 1;
    pipelineInfo.pStages = &vertStage;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = shadowPipelineLayout;
    pipelineInfo.renderPass = shadowRenderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    BGE_ASSERT_VKRESULT(vkCreateGraphicsPipelines(vkDevice.GetDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &shadowPipeline),
        "Failed to create shadow pipeline!");

    vkDestroyShaderModule(vkDevice.GetDevice(), vertShaderModule, nullptr);

    CreateSuccess("Shadow pipeline created!");
}

void VulkanRender::createViewportRenderPass()
{
    CreateInfo("Creating viewport renderpass...");

    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = viewportTexture->getFormat(); //CHECK
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = FindDepthFormat(vkDevice.GetPhysicalDevice());
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorRef{};
    colorRef.attachment = 0;
    colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthRef{};
    depthRef.attachment = 1;
    depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorRef;
    subpass.pDepthStencilAttachment = &depthRef;

    std::array<VkAttachmentDescription, 2> attachments = {
        colorAttachment,
        depthAttachment
    };

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    BGE_ASSERT_VKRESULT(
        vkCreateRenderPass(
            vkDevice.GetDevice(),
            &renderPassInfo,
            nullptr,
            &viewportRenderPass
        ),
        "Failed to create viewport render pass"
    );

    CreateSuccess("Viewport render pass created");
}

void VulkanRender::createViewportDepthResources(uint32_t width, uint32_t height)
{
    VkFormat format = FindDepthFormat(vkDevice.GetPhysicalDevice());

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent = { width, height, 1 };
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    vkCreateImage(vkDevice.GetDevice(), &imageInfo, nullptr, &viewportDepthImage);

    VkMemoryRequirements memReq;
    vkGetImageMemoryRequirements(vkDevice.GetDevice(), viewportDepthImage, &memReq);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = FindMemoryType(
        memReq.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        vkDevice.GetPhysicalDevice());

    vkAllocateMemory(vkDevice.GetDevice(), &allocInfo, nullptr, &viewportDepthMemory);
    vkBindImageMemory(vkDevice.GetDevice(), viewportDepthImage, viewportDepthMemory, 0);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = viewportDepthImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.layerCount = 1;

    vkCreateImageView(vkDevice.GetDevice(), &viewInfo, nullptr, &viewportDepthView);
}

void VulkanRender::RecordViewportCommandBuffer()
{
    VkCommandBuffer cmd = viewportCommandBuffer;

    vkResetCommandBuffer(cmd, 0);

    VkCommandBufferBeginInfo begin{};
    begin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    vkBeginCommandBuffer(cmd, &begin);

    VkClearValue clear[2];
    clear[0].color = { {0.15f,0.15f,0.18f,1.0f} };
    clear[1].depthStencil = { 1.0f,0 };

    VkRenderPassBeginInfo rp{};
    rp.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    rp.renderPass = viewportRenderPass;
    rp.framebuffer = viewportFramebuffer;
    rp.renderArea.extent = {
        static_cast<uint32_t>(viewport_width),
        static_cast<uint32_t>(viewport_height)
    };
    rp.clearValueCount = 2;
    rp.pClearValues = clear;

    vkCmdBeginRenderPass(cmd, &rp, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport vp{};
    vp.x = 0;
    vp.y = 0;
    vp.width = viewport_width;
    vp.height = viewport_height;
    vp.minDepth = 0.0f;
    vp.maxDepth = 1.0f;
    vkCmdSetViewport(cmd, 0, 1, &vp);

    VkRect2D scissor{};
    scissor.extent = {
        static_cast<uint32_t>(viewport_width),
        static_cast<uint32_t>(viewport_height)
    };
    vkCmdSetScissor(cmd, 0, 1, &scissor);

    vkCmdBindPipeline(cmd,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        vkPipeline.GetGraphicsPipeline());

    DrawMeshesForRecordCommandBuffer(cmd);

    vkCmdEndRenderPass(cmd);

    vkEndCommandBuffer(cmd);
}

void VulkanRender::initViewport()
{
    createViewportDepthResources(1280, 720);

    viewportTexture->CreateRenderTarget(
        vkDevice.GetDevice(),
        vkDevice.GetPhysicalDevice(),
        1280,
        720,
        viewportRenderPass
    );

    ImTextureID viewportDescriptor = (ImTextureID)ImGui_ImplVulkan_AddTexture(
        viewportTexture->GetSampler(),
        viewportTexture->GetImageView(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );

    viewportTexture->SetImGuiTexture(viewportDescriptor);
    VkImageView attachments[] =
    {
        viewportTexture->GetImageView(),
        viewportDepthView
    };

    VkFramebufferCreateInfo info{};
    info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    info.renderPass = viewportRenderPass;
    info.attachmentCount = 2;
    info.pAttachments = attachments;
    info.width = 1280;
    info.height = 720;
    info.layers = 1;

    vkCreateFramebuffer(vkDevice.GetDevice(), &info, nullptr, &viewportFramebuffer);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = vkCommandBuffer.GetCommandPool();
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;

    BGE_ASSERT_VKRESULT(
        vkAllocateCommandBuffers(
            vkDevice.GetDevice(),
            &allocInfo,
            &viewportCommandBuffer
        ),
        "Failed to allocate viewport command buffer"
    );
}

#endif