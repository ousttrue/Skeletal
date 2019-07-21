#include "texture.h"
#include <d3d11.h>
#include <wrl/client.h>
#include <plog/Log.h>
using namespace Microsoft::WRL;

namespace skeletal::dx11
{

class TextureImpl
{
    ComPtr<ID3D11ShaderResourceView> m_srv;

public:
    ID3D11ShaderResourceView *GetHandle() const { return m_srv.Get(); }
    void Bind(ID3D11DeviceContext *context, int slot)
    {
        ID3D11ShaderResourceView *srvList[] = {m_srv.Get()};
        context->PSSetShaderResources(slot, _countof(srvList), srvList);
    }
    void SetImage(ID3D11Device *device, int width, int height, int channels, const std::byte *data)
    {
        if (channels != 4)
        {
            LOGE << "channels is not 4: " << channels;
            return;
        }
        D3D11_TEXTURE2D_DESC desc;
        desc.Width = width;
        desc.Height = height;
        desc.MipLevels = 1;
        desc.ArraySize = 1;
        desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        desc.SampleDesc.Count = 1;
        desc.SampleDesc.Quality = 0;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA initData;
        initData.pSysMem = data;
        initData.SysMemPitch = width * channels;
        initData.SysMemSlicePitch = 0; // only 3D texture

        ComPtr<ID3D11Texture2D> texture;
        if (device->CreateTexture2D(&desc, &initData, &texture))
        {
            LOGE << "fail to create texture";
            return;
        }

        if (FAILED(device->CreateShaderResourceView(texture.Get(), nullptr, &m_srv)))
        {
            LOGE << "fail to create srv";
            return;
        }

        // OK
    }
};

Texture::Texture()
    : m_impl(new TextureImpl)
{
}

Texture::~Texture()
{
    delete m_impl;
}

void Texture::SetImage(ID3D11Device *device, int width, int height, int channels, const std::byte *data)
{
    m_impl->SetImage(device, width, height, channels, data);
}

void Texture::Bind(ID3D11DeviceContext *context, int slot)
{
    m_impl->Bind(context, slot);
}

void *Texture::GetHandle()
{
    return m_impl->GetHandle();
}

} // namespace skeletal::dx11