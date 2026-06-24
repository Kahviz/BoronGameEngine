#include "Image2d.h"

void Image2d::Draw(ImVec2 size)
{
    ImGui::Image(
        (ImTextureID)GetTexture(),
        size
    );
}

bool Image2d::LoadImGuiImage(IRenderer* renderer, const std::string& path)
{
    if (!m_isLoaded) {
        m_isLoaded = true;
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
}
