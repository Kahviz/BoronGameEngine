#include "BoronGui_implDX11.h"
#include "Backends.h"

BoronGuiNeeds BoronGui_implDX11::s_boronGuiNeeds{};
ComPtr<ID3D11Buffer> BoronGui_implDX11::s_vertexBuffer{};

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

    D3D11_BUFFER_DESC bufferDesc{};

    bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    bufferDesc.ByteWidth = sizeof(Vertex2d) * 1000;
    bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    BGE_ASSERT_HRESULT(s_boronGuiNeeds.device->CreateBuffer(
        &bufferDesc,
        nullptr,
        &s_vertexBuffer
    ), "Failed to create vertexBuffer");

    CreateInfo("BoronGui DX11 graphics init!");
}

void BoronGui_implDX11::ReSizeViewport(GPUVector2 p_newSize) {

}

void BoronGui_implDX11::SetBoronGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds) {
	s_boronGuiNeeds = p_boronGuiNeeds;
}

void BoronGui_implDX11::UpdatePerFrameOBJ(PerFrameStuct& p_perFrameStuct) {

}

void BoronGui_implDX11::UploadBatch(const std::vector<Vertex2d>& p_vertices, const std::vector<uint32_t>& p_indices) {
    D3D11_MAPPED_SUBRESOURCE mapped{};
    
    BGE_ASSERT_HRESULT(s_boronGuiNeeds.deviceContext->Map(
        s_vertexBuffer.Get(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mapped
    ), "Failed to map Vertex Buffer!");

    memcpy(mapped.pData, p_vertices.data(), p_vertices.size() * sizeof(Vertex2d));

    s_boronGuiNeeds.deviceContext->Unmap(s_vertexBuffer.Get(), 0);
}

void BoronGui_implDX11::DrawBatch() {

}
#endif

