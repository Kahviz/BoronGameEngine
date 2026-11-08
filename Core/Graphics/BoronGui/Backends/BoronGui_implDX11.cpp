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

ComPtr<ID3D11BlendState> BoronGui_implDX11::s_blendState{};

ComPtr<ID3D11RasterizerState> BoronGui_implDX11::s_rasterizerState{};

UINT BoronGui_implDX11::s_indexCount{};

#if DIRECTX11 == 1
void BoronGui_implDX11::Init() {
    CreateInfo("Initing BoronGui DX11 backend!");

    if (!s_boronGuiNeeds.device || !s_boronGuiNeeds.deviceContext) {
        CreateError("DX11 Device or Context is nullptr!");
        return;
    }

    InitShaders();
    InitGraphics();
    CreateRasterizerState();
    CreateBlendState();
    InitInputLayout();
}

void BoronGui_implDX11::InitInputLayout() {
    D3D11_INPUT_ELEMENT_DESC layout[] = {
         { "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof(Vertex2d, pos), D3D11_INPUT_PER_VERTEX_DATA, 0},
         { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, offsetof(Vertex2d, color), D3D11_INPUT_PER_VERTEX_DATA, 0}
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
    std::filesystem::path pixelShaderPath = std::filesystem::path(BORONGUI_SHADER_DIR) / "DX11" / "PixelShader.hlsl";

    BGE_ASSERT_HRESULT(D3DCompileFromFile(
        vertexShaderPath.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "vs_5_0",
        0,
        0,
        &s_vertexShaderBlob,
        &errorBlob
    ), "Failed to compile vertexShader");

    BGE_ASSERT_HRESULT(D3DCompileFromFile(
        pixelShaderPath.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        "main",
        "ps_5_0",
        0,
        0,
        &s_pixelShaderBlob,
        &errorBlob
    ),"Failed to compile pixerShader");

    BGE_ASSERT_HRESULT(
        s_boronGuiNeeds.device->CreateVertexShader(
            s_vertexShaderBlob->GetBufferPointer(),
            s_vertexShaderBlob->GetBufferSize(),
            nullptr,
            &s_vertexShader
        ),
        "Failed to create vertexShader!"
    );

    BGE_ASSERT_HRESULT(
        s_boronGuiNeeds.device->CreatePixelShader(
            s_pixelShaderBlob->GetBufferPointer(),
            s_pixelShaderBlob->GetBufferSize(),
            nullptr,
            &s_pixelShader
        ),
        "Failed to create pixelShader!"
    );

    CreateInfo("Vertex Shader compiled successfully!");
}

void BoronGui_implDX11::CreateBlendState() {
    D3D11_BLEND_DESC blendDesc{};

    blendDesc.RenderTarget[0].BlendEnable = true;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    BGE_ASSERT_HRESULT(s_boronGuiNeeds.device->CreateBlendState(&blendDesc, &s_blendState),"Failed to create Blend State");
}

void BoronGui_implDX11::CreateRasterizerState() {
    D3D11_RASTERIZER_DESC rasterizerDesc{};

    rasterizerDesc.CullMode = D3D11_CULL_NONE;
    rasterizerDesc.FillMode = D3D11_FILL_SOLID;
    rasterizerDesc.ScissorEnable = false;
    rasterizerDesc.DepthClipEnable = true;

    BGE_ASSERT_HRESULT(s_boronGuiNeeds.device->CreateRasterizerState(&rasterizerDesc, &s_rasterizerState), "Failed to create Rasterizer State");
}

void BoronGui_implDX11::InitGraphics() {
    D3D11_BUFFER_DESC vertexBufferDesc{};

    vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    vertexBufferDesc.ByteWidth = sizeof(Vertex2d) * 1000;
    vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    BGE_ASSERT_HRESULT(s_boronGuiNeeds.device->CreateBuffer(&vertexBufferDesc, nullptr, &s_vertexBuffer), "Failed to create vertexBuffer");

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

void BoronGui_implDX11::SetBoronGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds) {
	s_boronGuiNeeds = p_boronGuiNeeds;
}

void BoronGui_implDX11::UpdatePerFrameOBJ(PerFrameStuct& p_perFrameStuct) {

}

void BoronGui_implDX11::SetScissorRect(const BML::Vec2& p_pos, const BML::Vec2& p_size) {
    D3D11_RECT rect{};

    rect.left = static_cast<LONG>(p_pos.x());
    rect.top = static_cast<LONG>(p_pos.y());
    rect.right = static_cast<LONG>(p_pos.x() + p_size.x());
    rect.bottom = static_cast<LONG>(p_pos.y() + p_size.y());

    s_boronGuiNeeds.deviceContext->RSSetScissorRects(1, &rect);
}

void BoronGui_implDX11::ReSizeViewport(GPUVector2 p_newSize) {
    D3D11_VIEWPORT viewport{};

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = p_newSize.x;
    viewport.Height = p_newSize.y;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    s_boronGuiNeeds.deviceContext->RSSetViewports(1, &viewport);
}

void BoronGui_implDX11::UploadBatch(const std::vector<Vertex2d>& p_vertices, const std::vector<uint32_t>& p_indices) {
    s_indexCount = 0;

    D3D11_MAPPED_SUBRESOURCE mappedVertex{};
    D3D11_MAPPED_SUBRESOURCE mappedIndex{};

    if (p_vertices.empty()) {
        return;
    }
    
    if (p_indices.empty()) {
        return;
    }

    if (p_vertices.size() > 1000) { //for now
        return;
    }

    if (p_indices.size() > 1000) { //for now
        return;
    }

    BGE_ASSERT_HRESULT(s_boronGuiNeeds.deviceContext->Map(
        s_vertexBuffer.Get(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mappedVertex
    ), "Failed to map Vertex Buffer!");

    memcpy(mappedVertex.pData, p_vertices.data(), p_vertices.size() * sizeof(Vertex2d));

    s_boronGuiNeeds.deviceContext->Unmap(s_vertexBuffer.Get(), 0);

    BGE_ASSERT_HRESULT(s_boronGuiNeeds.deviceContext->Map(
        s_indexBuffer.Get(),
        0,
        D3D11_MAP_WRITE_DISCARD,
        0,
        &mappedIndex
    ), "Failed to map Index Buffer!");

    memcpy(mappedIndex.pData, p_indices.data(), p_indices.size() * sizeof(uint32_t));

    s_boronGuiNeeds.deviceContext->Unmap(s_indexBuffer.Get(), 0);

    s_indexCount = static_cast<UINT>(p_indices.size());
}

void BoronGui_implDX11::ReSizeViewport(const BML::Vec2& p_newSize) {
    D3D11_VIEWPORT viewport{};

    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.Width = p_newSize.x();
    viewport.Height = p_newSize.y();
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;

    s_boronGuiNeeds.deviceContext->RSSetViewports(1, &viewport);
}

void BoronGui_implDX11::DrawBatch() {
    if (s_indexCount == 0) {
        return;
    }

    ID3D11Buffer* vertexBuffer = s_vertexBuffer.Get();

    UINT stride = sizeof(Vertex2d);
    UINT offset = 0;

    s_boronGuiNeeds.deviceContext->RSSetState(s_rasterizerState.Get());

    s_boronGuiNeeds.deviceContext->OMSetBlendState(s_blendState.Get(), nullptr, 0xFFFFFFFF);

    s_boronGuiNeeds.deviceContext->IASetInputLayout(s_inputLayout.Get());
    s_boronGuiNeeds.deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    s_boronGuiNeeds.deviceContext->IASetIndexBuffer(s_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    s_boronGuiNeeds.deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    s_boronGuiNeeds.deviceContext->VSSetShader(s_vertexShader.Get(), nullptr, 0);
    s_boronGuiNeeds.deviceContext->PSSetShader(s_pixelShader.Get(), nullptr, 0);

    s_boronGuiNeeds.deviceContext->DrawIndexed(s_indexCount, 0, 0);
}
#endif
