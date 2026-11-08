#pragma once

#include "GLOBALS.h"

#if DIRECTX11 == 1
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"d3dcompiler.lib")

#include "Backends.h"

class BoronGui_implDX11 : public BoronGuiBackends::Backends {
public:
    void Init() override;
    void InitGraphics();
    void ReSizeViewport(GPUVector2 p_newSize) override;
    void SetBoronGuiNeeds(BoronGuiNeeds& p_boronGuiNeeds) override;
    void UpdatePerFrameOBJ(PerFrameStuct& p_perFrameStuct) override;

    void UploadBatch(const std::vector<Vertex2d>& p_vertices, const std::vector<uint32_t>& p_indices) override;
    void DrawBatch() override;

    void InitInputLayout();
    void InitShaders();
private:
    static BoronGuiNeeds s_boronGuiNeeds;
    static ComPtr<ID3D11Buffer> s_vertexBuffer;
    static ComPtr<ID3D11Buffer> s_indexBuffer;
    static ComPtr<ID3D11InputLayout> s_inputLayout;

    static ComPtr<ID3D11VertexShader> s_vertexShader;
    static ComPtr<ID3D11PixelShader> s_pixelShader;

    static ComPtr<ID3DBlob> s_vertexShaderBlob;
    static ComPtr<ID3DBlob> s_pixelShaderBlob;
};
#endif