#include "Texture.h"
#include <iostream>

bool Texture::Load(std::string path)
{
    std::filesystem::path fsPath(path);

    if (!std::filesystem::exists(path)) {
        MakeAError("Texture Doesnt Exist!");

        return false;
    }
    if (fsPath.extension() == ".png" || fsPath.extension() == ".png")
    {
        return false;
    }

    #if DIRECTX11 == 1
        ID3D11Device* device = m_Renderer->GetDevice();
        ID3D11DeviceContext* context = m_Renderer->GetDeviceContext();

        ComPtr<ID3D11Resource> resource = nullptr;
        DX::Check(DirectX::CreateWICTextureFromFile(device, context, path.c_str(), resource.ReleaseAndGetAddressOf(), m_DiffuseTexture.ReleaseAndGetAddressOf()));
    #endif
    return false;
}
