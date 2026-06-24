#pragma once
#include <string>
#include "GLOBALS.h"
#include "Texture.h"
#include "imgui.h"
#include "IRenderer.h"
#include "GraphicsBackends.h"

class Image2d {
public:
	void Draw(ImVec2 size);
	bool LoadImGuiImage(IRenderer* renderer, const std::string& path);

	#if VULKAN == 1
		VkDescriptorSet& GetTexture() { return m_descriptorSet; };
	#endif
	#if DIRECTX11 == 1
		ID3D11ShaderResourceView* GetTexture() { return srv.Get(); };
	#endif
	bool IsLoaded() {
		return m_isLoaded;
	}
private:
	Texture m_texture;
	bool m_isLoaded = false;
	#if VULKAN == 1
		VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
	#endif
	#if DIRECTX11 == 1
		ComPtr<ID3D11ShaderResourceView> srv;
	#endif
};