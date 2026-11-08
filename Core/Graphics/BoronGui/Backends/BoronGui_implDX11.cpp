#include "BoronGui_implDX11.h"

#include <filesystem>

#include "Backends.h"

BoronGuiNeeds BoronGui_implDX11::s_boronGuiNeeds{};

ComPtr<ID3D11Buffer> BoronGui_implDX11::s_vertexBuffer{};
ComPtr<ID3D11Buffer> BoronGui_implDX11::s_indexBuffer{};

ComPtr<ID3D11InputLayout> BoronGui_implDX11::s_inputLayout{};

ComPtr<ID3D11VertexShader> BoronGui_implDX11::s_vertexShader{};
ComPtr<ID3D11PixelShader> BoronGui_implDX11::s_pixelShader{};

ComPtr<ID3DBlob> BoronGui_implDX11::s_vertexShaderBlob{};
ComPtr<ID3DBlob> BoronGui_implDX11::s_pixelShaderBlob{};

#if DIRECTX11 == 1
void BoronGui_implDX11::Init() {
    CreateInfo("Initing BoronGui DX11 backend!");

    InitShaders();
    InitGraphics();
    InitInputLayout();
}

void BoronGui_implDX11::InitInputLayout() {
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex2d, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex2d, color), D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    BGE_ASSERT_HRESULT(
        s_boronGuiNeeds.device->CreateInputLayout(
            layout,
            ARRAYSIZE(layout),
            s_vertexShaderBlob->GetBufferPointer(),
            s_vertexShaderBlob->GetBufferSize(),
            &s_inputLayout
        ), "Failed to create Input Layout"
    );
}

void BoronGui_implDX11::InitShaders() {
    ComPtr<ID3DBlob> errorBlob;

    std::filesystem::path vertexShaderPath = std::filesystem::path(BORONGUI_SHADER_DIR) / "DX11" / "VertexShader.hlsl";

    HRESULT hr = D3DCompileFromFile(
        vertexShaderPath.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "vs_5_0",
        0,
        0,
        &s_vertexShaderBlob,
        &errorBlob
    );

    if (FAILED(hr)) {
        CreateError(std::format("Shader HRESULT: 0x{:08X}", (uint32_t)hr));

        if (errorBlob) {
            CreateError( (const char*)errorBlob->GetBufferPointer());
        }

        return;
    }

    if (!s_vertexShaderBlob) {
        CreateError("Shader Blob is nullptr!");
        return;
    }

    BGE_ASSERT_HRESULT(
        s_boronGuiNeeds.device->CreateVertexShader(
            s_vertexShaderBlob->GetBufferPointer(),
            s_vertexShaderBlob->GetBufferSize(),
            nullptr,
            &s_vertexShader
        ),
        "Failed to create Vertex Shader!"
    );

    CreateInfo("Vertex Shader compiled successfully!");
}
void BoronGui_implDX11::InitGraphics() {
    if (!s_boronGuiNeeds.device || !s_boronGuiNeeds.deviceContext) {
        CreateError("DX11 Device or Context is nullptr!");
        return;
    }

    D3D11_BUFFER_DESC vertexBufferDesc{};

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(Vertex2d) * 1000;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    BGE_ASSERT_HRESULT(s_boronGuiNeeds.device->CreateBuffer(
        &vertexBufferDesc,
        nullptr,
        &s_vertexBuffer
    ), "Failed to create vertexBuffer");

    D3D11_BUFFER_DESC indexBufferDesc{};

    indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    indexBufferDesc.ByteWidth = sizeof(uint32_t) * 1000;
    indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    BGE_ASSERT_HRESULT(s_boronGuiNeeds.device->CreateBuffer(
        &indexBufferDesc,
        nullptr,
        &s_indexBuffer
    ), "Failed to create indexBuffer");

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
    if (p_vertices.empty()) {
        return;
    }
        

    if (p_vertices.size() > 1000) { //for now
        return;
    }

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