#include "BoronGui_implDX11.h"
#include "Backends.h"

BoronGuiNeeds BoronGui_implDX11::s_boronGuiNeeds{};

#if DIRECTX11 == 1
void BoronGui_implDX11::Init() {
	CreateInfo("Initing BoronGui DX11 backend!");

	InitGraphics();
}

void BoronGui_implDX11::InitGraphics() {
    if (!s_boronGuiNeeds.device || !s_boronGuiNeeds.deviceContext) {
        CreateError("DX11 Device or Context is nullptr!");
        return;
    }

    CreateInfo("BoronGui DX11 graphics initialized!");
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

