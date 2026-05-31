#pragma once
#include "GLOBALS.h"

#if VULKAN == 1
	#include "VulkanRender.h"
	#include "backends/imgui_impl_vulkan.h"
#endif

#if DIRECTX11 == 1
	#include <d3d11.h>
	#include <DirectXMath.h>
	#include <wrl/client.h>
	#include <wincodec.h>
	#include <d3dcompiler.h>
	#include "Dx11Renderer.h"
	#include "backends/imgui_impl_dx11.h"
#endif