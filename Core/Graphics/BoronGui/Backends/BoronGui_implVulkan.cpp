#include "BoronGui_implVulkan.h"

#if VULKAN == 1
#include <Logger/Logger.h>

#include "Shaders/Vulkan/FragmentShader.h"
#include "Shaders/Vulkan/VertexShader.h"

#include "Vulkan/VulkanHelpers.h"

#include "Vertex2d.h"
#include "Widgets/Widgets.h"
#include "GuiTexture/GuiTextureManager.h"

BoronGuiNeeds BoronGui_implVulkan::s_boronGuiNeeds{};
VkShaderModule BoronGui_implVulkan::s_vertShaderModule = VK_NULL_HANDLE;
VkShaderModule BoronGui_implVulkan::s_fragShaderModule = VK_NULL_HANDLE;
VkPipelineLayout BoronGui_implVulkan::s_pipelineLayout = VK_NULL_HANDLE;
VkPipeline BoronGui_implVulkan::s_graphicsPipeline = VK_NULL_HANDLE;
VulkanBuffer BoronGui_implVulkan::s_vkBuffer{};
VulkanBuffer BoronGui_implVulkan::s_vkBufferIndex{};
VkIndexType BoronGui_implVulkan::s_indexType = VK_INDEX_TYPE_UINT32;
VkCommandBuffer BoronGui_implVulkan::s_commandBuffer;
BoronGui_implVulkan::GlobalPushConstant BoronGui_implVulkan::s_globalPushConstant{};
uint32_t BoronGui_implVulkan::s_indexCount = 0;
uint32_t BoronGui_implVulkan::s_currentObjectCount = 1024;
VkDescriptorSet BoronGui_implVulkan::s_textureDescriptorSet{};
VkDescriptorPool BoronGui_implVulkan::s_descriptorPool{};
VkDescriptorSetLayout BoronGui_implVulkan::s_textureLayout{};

void BoronGui_implVulkan::BeginFrame() {

}

void BoronGui_implVulkan::SetupRenderState(VkCommandBuffer commandBuffer) {
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, s_graphicsPipeline);

    /*if (GuiTextureManager::getTextureCount() != s_currentObjectCount) {
        resizeDescriptorPool(GuiTextureManager::getTextureCount());
        resizeTextureDescriptorSet();
    }*/

    VkViewport viewport{};
    viewport.height = s_boronGuiNeeds.swapchainExtent.height;
    viewport.width = s_boronGuiNeeds.swapchainExtent.width;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.extent = { s_boronGuiNeeds.swapchainExtent.width, s_boronGuiNeeds.swapchainExtent.height };

    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    std::vector<VkDescriptorImageInfo> descriptorImageInfos{};

    for (const auto& texture : GuiTextureManager::getTextures()) {
        VkDescriptorImageInfo imageInfo{};

        imageInfo.imageView = texture.GetImageView();
        imageInfo.sampler = texture.GetSampler();
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        descriptorImageInfos.push_back(imageInfo);
    }

    VkWriteDescriptorSet writeDescriptorSet{};

    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.descriptorCount = descriptorImageInfos.size();
    writeDescriptorSet.dstBinding = 0;
    writeDescriptorSet.dstSet = s_textureDescriptorSet;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSet.pImageInfo = descriptorImageInfos.data();
    writeDescriptorSet.dstArrayElement = 1;

    if (!descriptorImageInfos.empty() && s_textureDescriptorSet != VK_NULL_HANDLE) {
        vkUpdateDescriptorSets(
            s_boronGuiNeeds.device,
            1,
            &writeDescriptorSet,
            0,
            0
        );
    }

    vkCmdBindDescriptorSets(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        s_pipelineLayout,
        0,
        1,
        &s_textureDescriptorSet,
        0,
        nullptr
    );
}

void BoronGui_implVulkan::EndFrame() {
}

void BoronGui_implVulkan::Init() {
	CreateInfo("Initing BoronGui Vulkan backend!");
    
    createDescriptorPool(s_currentObjectCount);
    createTextureDescriptorSet();
    allocateDescriptorSet();
    InitPipeline();
}

void BoronGui_implVulkan::resizeDescriptorPool(uint32_t p_newMaxTextures) {
    s_currentObjectCount = p_newMaxTextures;

    if (s_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(s_boronGuiNeeds.device, s_descriptorPool, nullptr);
    }

    std::array<VkDescriptorPoolSize, 1> poolSizes{};

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[0].descriptorCount = p_newMaxTextures;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 1;

    BGE_ASSERT_VKRESULT(
        vkCreateDescriptorPool(
            s_boronGuiNeeds.device,
            &poolInfo,
            nullptr,
            &s_descriptorPool
        ),
        "Failed to create descriptor pool"
    );

    allocateDescriptorSet();
}

void BoronGui_implVulkan::destroyDescriptorPool() {
    if (s_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(s_boronGuiNeeds.device, s_descriptorPool, nullptr);
        s_descriptorPool = VK_NULL_HANDLE;
    }
}

void BoronGui_implVulkan::createTextureDescriptorSet() {
    VkDescriptorSetLayoutBinding textureBinding{};
    textureBinding.binding = 0;
    textureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureBinding.descriptorCount = s_currentObjectCount;
    textureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &textureBinding;

    BGE_ASSERT_VKRESULT(vkCreateDescriptorSetLayout(s_boronGuiNeeds.device, &layoutInfo, nullptr, &s_textureLayout), "Failed to create descriptor!");
}

void BoronGui_implVulkan::resizeTextureDescriptorSet() {
    if (s_textureLayout != VK_NULL_HANDLE) {
        vkDestroyDescriptorSetLayout(s_boronGuiNeeds.device, s_textureLayout, nullptr);
        s_textureLayout = VK_NULL_HANDLE;
    }

    VkDescriptorSetLayoutBinding textureBinding{};
    textureBinding.binding = 0;
    textureBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    textureBinding.descriptorCount = s_currentObjectCount;
    textureBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &textureBinding;

    BGE_ASSERT_VKRESULT(vkCreateDescriptorSetLayout(s_boronGuiNeeds.device, &layoutInfo, nullptr, &s_textureLayout), "Failed to create descriptor!");
}

void BoronGui_implVulkan::updateTextureDescriptors() {
    
}

void BoronGui_implVulkan::createDescriptorPool(uint32_t p_maxObjects) {
    std::array<VkDescriptorPoolSize, 1> poolSizes{};

    poolSizes[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[0].descriptorCount = p_maxObjects;

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = 1;

    BGE_ASSERT_VKRESULT(
        vkCreateDescriptorPool(
            s_boronGuiNeeds.device,
            &poolInfo,
            nullptr,
            &s_descriptorPool
        ),
        "Failed to create descriptor pool"
    );

    s_currentObjectCount = p_maxObjects;
}

const BoronGuiNeeds& BoronGui_implVulkan::getGuiNeeds() {
	return s_boronGuiNeeds;
}

void BoronGui_implVulkan::allocateDescriptorSet() {
    VkDescriptorSetAllocateInfo vkDescriptorSetAllocateInfo{};

    vkDescriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    vkDescriptorSetAllocateInfo.pNext = nullptr;
    vkDescriptorSetAllocateInfo.descriptorPool = s_descriptorPool;
    vkDescriptorSetAllocateInfo.descriptorSetCount = 1;
    vkDescriptorSetAllocateInfo.pSetLayouts = &s_textureLayout;

    BGE_ASSERT_VKRESULT(vkAllocateDescriptorSets(s_boronGuiNeeds.device, &vkDescriptorSetAllocateInfo, &s_textureDescriptorSet),
        "Failed to allocate for descriptors");
}

void BoronGui_implVulkan::ReSizeViewport(GPUVector2 p_newSize) {
    s_boronGuiNeeds.swapchainExtent.width = static_cast<uint32_t>(p_newSize.x);
    s_boronGuiNeeds.swapchainExtent.height = static_cast<uint32_t>(p_newSize.y);
}

void BoronGui_implVulkan::SetBoronGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds) {
    s_boronGuiNeeds = p_boronGuiNeeds;
}

void BoronGui_implVulkan::UpdatePerFrameOBJ(PerFrameStuct& p_perFrameStuct) {
    s_commandBuffer = p_perFrameStuct.commandBuffer;
}

void BoronGui_implVulkan::UploadBatch(const std::vector<Vertex2d>& p_vertices, const std::vector<uint32_t>& p_indices) {
    if (p_vertices.empty() || p_indices.empty()) {
        s_indexCount = 0;
        return;
    }

    s_globalPushConstant.viewportSize = {
        static_cast<float>(s_boronGuiNeeds.swapchainExtent.width),
        static_cast<float>(s_boronGuiNeeds.swapchainExtent.height)
    };

    vkCmdPushConstants(
        s_commandBuffer,
        s_pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        sizeof(GlobalPushConstant),
        &s_globalPushConstant
    );

    s_indexCount = static_cast<uint32_t>(p_indices.size());

    const VkDeviceSize vertexSize =
        p_vertices.size() * sizeof(Vertex2d);

    const VkDeviceSize indexSize =
        p_indices.size() * sizeof(uint32_t);

    static VkDeviceSize lastVertexSize = 0;
    static VkDeviceSize lastIndexSize = 0;

    const bool buffersCreated =
        s_vkBuffer.IsCreated() &&
        s_vkBufferIndex.IsCreated();

    if (!buffersCreated) [[unlikely]] {
        s_vkBuffer.Create(
            s_boronGuiNeeds.device,
            s_boronGuiNeeds.physicalDevice,
            vertexSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

        s_vkBufferIndex.Create(
            s_boronGuiNeeds.device,
            s_boronGuiNeeds.physicalDevice,
            indexSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
        );

    }
    else if (lastVertexSize != vertexSize || lastIndexSize != indexSize) [[unlikely]] {
        s_vkBuffer.Resize(
            vertexSize,
            s_boronGuiNeeds.commandPool,
            s_boronGuiNeeds.graphicsQueue
        );

        s_vkBufferIndex.Resize(
            indexSize,
            s_boronGuiNeeds.commandPool,
            s_boronGuiNeeds.graphicsQueue
        );
    }

    s_vkBuffer.UploadData(p_vertices.data(), vertexSize);
    s_vkBufferIndex.UploadData(p_indices.data(), indexSize);

    lastVertexSize = vertexSize;
    lastIndexSize = indexSize;

    VkBuffer vertexBuffer = s_vkBuffer.GetBuffer();
    VkDeviceSize offset = 0;

    vkCmdBindVertexBuffers(
        s_commandBuffer,
        0,
        1,
        &vertexBuffer,
        &offset
    );

    vkCmdBindIndexBuffer(
        s_commandBuffer,
        s_vkBufferIndex.GetBuffer(),
        0,
        VK_INDEX_TYPE_UINT32
    );
}

void BoronGui_implVulkan::DrawBatch() {
    vkCmdDrawIndexed(
        s_commandBuffer,
        s_indexCount,
        1,
        0,
        0,
        0
    );
}

bool BoronGui_implVulkan::InitPipeline() {
    CreateInfo("Initing VulkanPipeline!");
    
    auto vertShaderCode = ReadShader(VertexShader);
    auto fragShaderCode = ReadShader(FragmentShader);

    s_vertShaderModule = CreateShaderModule(s_boronGuiNeeds.device, vertShaderCode);
    s_fragShaderModule = CreateShaderModule(s_boronGuiNeeds.device, fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = s_vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = s_fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    auto bindingDescription = Vertex2d::getBindingDescription();
    auto attributeDescriptions = Vertex2d::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 0;
    vertexInputInfo.pVertexBindingDescriptions = nullptr;

    vertexInputInfo.vertexAttributeDescriptionCount = 0;
    vertexInputInfo.pVertexAttributeDescriptions = nullptr;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.cullMode = VK_CULL_MODE_NONE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;

    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    //PushConstant
    VkPushConstantRange globalPushConstant = CreatePushConstantRange(
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(GlobalPushConstant)
    );

    std::vector<VkPushConstantRange> pushConstants;

    pushConstants.push_back(globalPushConstant);
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &s_textureLayout;
    pipelineLayoutInfo.pPushConstantRanges = pushConstants.data();
    pipelineLayoutInfo.pushConstantRangeCount = pushConstants.size();

    BGE_ASSERT_VKRESULT(vkCreatePipelineLayout(s_boronGuiNeeds.device, &pipelineLayoutInfo, nullptr, &s_pipelineLayout), "Failed to create pipeline layout!");

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

    VkDynamicState dynamicStates[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    dynamicState.dynamicStateCount = 2;
    dynamicState.pDynamicStates = dynamicStates;

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = s_pipelineLayout;
    pipelineInfo.renderPass = s_boronGuiNeeds.renderPass;
    pipelineInfo.subpass = 0;

    BGE_ASSERT_VKRESULT(vkCreateGraphicsPipelines(s_boronGuiNeeds.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &s_graphicsPipeline), "Failed to create graphics pipeline!");

    return true;
}
#endif