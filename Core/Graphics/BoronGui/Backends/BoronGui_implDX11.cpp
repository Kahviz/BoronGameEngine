#include "BoronGui_implDX11.h"
#include "Backends.h"

BoronGuiNeeds BoronGui_implDX11::s_boronGuiNeeds{};

#if DIRECTX11 == 1
void BoronGui_implDX11::Init() {
	CreateInfo("Initing BoronGui DX11 backend!");

	InitGraphics();
}

void BoronGui_implDX11::InitGraphics() {
    HRESULT hr = D3D11CreateDevice(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &s_boronGuiNeeds.device,
        nullptr,
        &s_boronGuiNeeds.deviceContext
    );

    BGE_ASSERT_HRESULT(hr, "Failed to create DX11 device!");

    CreateInfo("DX11 Device created successfully!");
}

void BoronGui_implDX11::ReSizeViewport(GPUVector2 p_newSize) {

}

void BoronGui_implDX11::SetBoronGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds) {
	s_boronGuiNeeds = p_boronGuiNeeds;
}

void BoronGui_implDX11::UpdatePerFrameOBJ(PerFrameStuct& p_perFrameStuct) {

}

void BoronGui_implDX11::UploadBatch(const std::vector<Vertex2d>& p_vertices, const std::vector<uint32_t>& p_indices) {

}

void BoronGui_implDX11::DrawBatch() {

}
#endif

