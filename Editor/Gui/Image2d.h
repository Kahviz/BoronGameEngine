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
		VkDescriptorSet& GetDescriptorSet() { return m_descriptorSet; };
	#endif
	#if DIRECTX11 == 1
		ID3D11ShaderResourceView* GetSRV() { return srv.Get(); };
	#endif
private:
	Texture m_texture;

	#if VULKAN == 1
		VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
	#endif
	#if DIRECTX11 == 1
		ComPtr<ID3D11ShaderResourceView> srv;
	#endif
};