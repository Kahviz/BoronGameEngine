#pragma once
#include "GLOBALS.h"

#if VULKAN == 1
#include "vulkan/Vulkan.h"
#include "VulkanBuffer.h"
#include "Backends.h"
#include "BoronGuiTypes.h"

class BoronGui_implVulkan : public BoronGuiBackends::Backends {
public:
    static void BeginFrame();
    static void SetupRenderState(VkCommandBuffer commandBuffer);
    static void EndFrame();
    static const BoronGuiNeeds& GetGuiNeeds();

    void Init() override;
    void SetBoronGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds) override;
    void UpdatePerFrameOBJ(PerFrameStuct& p_perFrameStuct) override;
    void RenderAFrame() override;

    static bool InitPipeline();
private:
    static VkShaderModule m_vertShaderModule;
    static VkShaderModule m_fragShaderModule;
    static BoronGuiNeeds m_boronGuiNeeds;
    static VkPipelineLayout m_pipelineLayout;
    static VkPipeline m_graphicsPipeline;
    static VulkanBuffer m_vkBuffer; // This is just for test
    static VulkanBuffer m_vkBufferIndex; // This is just for test

    static VkIndexType indexType;

    static VkCommandBuffer m_commandBuffer;
};
#endif