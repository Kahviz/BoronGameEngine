#pragma once
#include "GLOBALS.h"
#include "Vulkan.h"

#if VULKAN == 1
#include "VulkanBuffer.h"
#include "Backends.h"
#include "BoronGuiTypes.h"
#include "BoronMathLibrary.h"

class BoronGui_implVulkan : public BoronGuiBackends::Backends {
public:
    static void BeginFrame();
    static void SetupRenderState(VkCommandBuffer commandBuffer);
    static void EndFrame();
    static void resizeDescriptorPool(uint32_t p_newMaxTextures);
    static void destroyDescriptorPool();
    static void createDescriptorPool(uint32_t p_maxObjects);
    static void createTextureDescriptorSet();
    static void resizeTextureDescriptorSet();
    static void updateTextureDescriptors();
    static const BoronGuiNeeds& getGuiNeeds();
    static void allocateDescriptorSet();
    void Init() override;
    void ReSizeViewport(GPUVector2 p_newSize) override;
    void SetBoronGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds) override;
    void UpdatePerFrameOBJ(PerFrameStuct& p_perFrameStuct) override;

    void UploadBatch(const std::vector<Vertex2d>& p_vertices, const std::vector<uint32_t>& p_indices) override;
    void DrawBatch() override;

    static bool InitPipeline();
private:
    struct GlobalPushConstant {
        GPUVector2 viewportSize{};
    };

    static GlobalPushConstant s_globalPushConstant;
    static VkShaderModule s_vertShaderModule;
    static VkShaderModule s_fragShaderModule;
    static BoronGuiNeeds s_boronGuiNeeds;
    static VkPipelineLayout s_pipelineLayout;
    static VkPipeline s_graphicsPipeline;
    static VulkanBuffer s_vkBuffer;
    static VulkanBuffer s_vkBufferIndex;

    static VkIndexType s_indexType;

    static VkCommandBuffer s_commandBuffer;

    static uint32_t s_indexCount;
    static uint32_t s_currentObjectCount;
    static VkDescriptorSet s_textureDescriptorSet;

    static VkDescriptorPool s_descriptorPool;

    static VkDescriptorSetLayout s_textureLayout;
};
#endif