#include "BoronGui_implDX11.h"
#include "Backends.h"

#if DIRECTX11 == 1
void BoronGui_implDX11::Init() {
}

void BoronGui_implDX11::ReSizeViewport(GPUVector2 p_newSize) {
}

void BoronGui_implDX11::SetBoronGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds) {
}

void BoronGui_implDX11::UpdatePerFrameOBJ(PerFrameStuct& p_perFrameStuct) {
}

void BoronGui_implDX11::UploadBatch(const std::vector<Vertex2d>& p_vertices, const std::vector<uint32_t>& p_indices) {
}

void BoronGui_implDX11::DrawBatch() {
}
#endif

