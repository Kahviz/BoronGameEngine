#pragma once
#include <string>
#include <filesystem>
#include "ErrorHandling/ErrorMessage.h"
#include "GLOBALS.h"
#include "Releaser.h"
#include "Debugging/Functions/BGE_ASSERTS.h"
#include "GraphicsBackends.h"

#if VULKAN == 1
    #include "Vulkan/vulkan.h"

    class VulkanRender;
#endif
class IRenderer;

class Texture {
public:
    int GetWidth() const { return m_width; }
    int GetHeight() const { return m_height; }
#if DIRECTX11 == 1
    Texture() : pTexture(nullptr) {}

    void SetSRV(ID3D11ShaderResourceView* srv);

    ID3D11ShaderResourceView* Load(std::string path, IRenderer& renderer);

    ID3D11ShaderResourceView* GetSRV() const {
        return pTexture.Get();
    }

    ID3D11ShaderResourceView* const* GetAddressOf() const {
        return pTexture.GetAddressOf();
    }

    const ComPtr<ID3D11ShaderResourceView>& GetTextureComPtr() const {
        return pTexture;
    }

    ComPtr<ID3D11ShaderResourceView>& GetTextureComPtr() {
        return pTexture;
    }
#endif

#if VULKAN == 1
    VkImageView GetImageView() const { return m_imageView; }
    VkSampler GetSampler() const { return m_sampler; }

    bool LoadVK(const std::string& path, IRenderer& renderer);
    void CreateImage(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties);
    void TransitionImageLayout(VkCommandBuffer cmd, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void CopyBufferToImage(VkCommandBuffer cmd, VkBuffer buffer, uint32_t width, uint32_t height);
    void Cleanup(VkDevice device);
    void CreateRenderTarget(VkDevice device, VkPhysicalDevice physicalDevice, uint32_t width, uint32_t height, VkRenderPass renderPass);
    ImTextureID getImGuiTexture() const { return m_ImGuiTexture; }
    VkFramebuffer getFramebuffer() const { return m_framebuffer; }
    VkFormat getFormat() const { return m_format; }
    void SetImGuiTexture(ImTextureID tex) { m_ImGuiTexture = tex; }
#endif
    bool IsLoadedConst() const {
        return Loaded;
    }
    bool IsLoaded() {
        if (this != nullptr) {
            return Loaded;
        }
    }
private:
    bool Loaded = false;
#if DIRECTX11 == 1
    ComPtr<ID3D11ShaderResourceView> pTexture;
#endif
#if VULKAN == 1
    VkImage m_image = VK_NULL_HANDLE;
    VkDeviceMemory m_imageMemory = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;
    VkSampler m_sampler = VK_NULL_HANDLE;
    ImTextureID m_ImGuiTexture = {};
    VkFramebuffer m_framebuffer = VK_NULL_HANDLE;
    VkRenderPass m_renderPass = VK_NULL_HANDLE;

    VkFormat m_format = VK_FORMAT_R8G8B8A8_UNORM;
#endif
    int m_width = 0;
    int m_height = 0;
};