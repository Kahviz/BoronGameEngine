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
#endif