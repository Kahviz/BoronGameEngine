#pragma once

#include "GLOBALS.h"

#if DIRECTX11 == 1
	#include <d3d11.h>
	#include <DirectXMath.h>
	#include <wrl/client.h>
	#include <wincodec.h>
	#include <d3dcompiler.h>

	//Imgui
	#include "backends/imgui_impl_dx11.h"

	//structs
	struct MeshStruct {
		ID3D11Device* device{};
	};

	struct MeshDrawStruct {
		ID3D11DeviceContext* ctx{};
	};
#endif