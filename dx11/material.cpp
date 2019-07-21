#include "material.h"
#include "shader.h"
#include "texture.h"
#include <d3d11.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;

namespace skeletal::dx11
{

class MaterialImpl
{
    ComPtr<ID3D11RasterizerState> m_rs;

public:
    void Activate(ID3D11Device *device, ID3D11DeviceContext *context)
    {
        if (!m_rs)
        {
            D3D11_RASTERIZER_DESC rasterizerDesc = {0};
            rasterizerDesc.FillMode = D3D11_FILL_SOLID;
            rasterizerDesc.CullMode = D3D11_CULL_BACK;
            rasterizerDesc.FrontCounterClockwise = true;
            if (FAILED(device->CreateRasterizerState(&rasterizerDesc, &m_rs)))
            {
                return;
            }
        }
        context->RSSetState(m_rs.Get());

        context->OMSetBlendState(nullptr, nullptr, 0xffffffff);
        context->OMSetDepthStencilState(nullptr, 0);
    }
};

Material::Material()
    : m_impl(new MaterialImpl)
{
}

Material::~Material()
{
    delete m_impl;
}

void Material::Activate(ID3D11Device *device, ID3D11DeviceContext *context)
{
    // constant buffer
    Shader->Activate(context);

    m_impl->Activate(device, context);

    // ToDo: SetConstants

    if (Texture)
    {
        Texture->Activate(context, 0);
    }
}

} // namespace skeletal::dx11
