#include "Image2d.h"
#include "IRenderer.h"

void Image2d::Draw(ImVec2 size)
{
    ImGui::Image(
        (ImTextureID)GetTexture(),
        size
    );
}

bool Image2d::LoadImGuiImage(
    IRenderer* renderer,
    const std::string& path)
{
    if (m_isLoaded)
        return true;

    #if VULKAN == 1

        if (renderer == nullptr)
            return false;

        if (!m_texture.LoadVK(path, *renderer))
            return false;

        m_descriptorSet = ImGui_ImplVulkan_AddTexture(
            m_texture.GetSampler(),
            m_texture.GetImageView(),
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        );

        if (m_descriptorSet == VK_NULL_HANDLE)
            return false;

        m_isLoaded = true;
        return true;

    #elif DIRECTX11 == 1

        if (renderer == nullptr)
            return false;

        if (!m_texture.Load(path, *renderer))
            return false;

        srv = m_texture.GetSRV();

        if (srv == nullptr)
            return false;

        m_isLoaded = true;
        return true;

    #endif

    return false;
}
