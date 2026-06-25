#pragma once
#include "GLOBALS.h"

#if VULKAN == 1
	#include "VulkanRender.h"
	#include "backends/imgui_impl_vulkan.h"
#endif

#ifdef _WIN32
	#include <backends/imgui_impl_win32.h>
#endif

#if DIRECTX11 == 1
	#include <backends/imgui_impl_dx11.h>
	#include <d3d11.h>
	#include <DirectXMath.h>
	#include <wrl/client.h>
	#include <wincodec.h>
	#include <d3dcompiler.h>
	#include "Dx11Renderer.h"
	#include "backends/imgui_impl_dx11.h"
#endif