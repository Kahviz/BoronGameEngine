#include "Image2d.h"

void Image2d::Draw(ImVec2 size)
{
    #if VULKAN == 1
        ImGui::Image(
            (ImTextureID)GetDescriptorSet(),
            size
        );
    #endif
    #if DIRECTX11 == 1
        ImGui::Image(
            (ImTextureID)GetSRV(),
            size
        );
    #endif
}

bool Image2d::LoadImGuiImage(IRenderer* renderer, const std::string& path)
{
#if VULKAN == 1
    if (!m_texture.LoadVK(path, *renderer))
        return false;

    m_descriptorSet = ImGui_ImplVulkan_AddTexture(
        m_texture.GetSampler(),
        m_texture.GetImageView(),
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
    );

    return true;
#endif
#if DIRECTX11 == 1
    if (!m_texture.Load(path, *renderer))
        return false;

    srv = m_texture.GetSRV();

    return true;
#endif
}
