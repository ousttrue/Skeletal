#include "shader.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#include <wrl/client.h>
#include <plog/Log.h>

using namespace Microsoft::WRL;

const std::string g_unlit =
#include "../shaders/unlit.hlsl"
    ;

namespace skeletal::dx11
{

class ShaderImpl
{
public:
    ComPtr<ID3D11VertexShader> m_vs;
    ComPtr<ID3D11Buffer> m_vsConstants;
    ComPtr<ID3D11InputLayout> m_inputLayout;
    ComPtr<ID3D11PixelShader> m_ps;
    ComPtr<ID3D11Buffer> m_psConstants;

    void SetVSConstantBuffer(ID3D11DeviceContext *deviceContext, const ConstantBuffer &buffer)
    {
        if (m_vsConstants)
        {
            deviceContext->UpdateSubresource(m_vsConstants.Get(), 0, nullptr, &buffer, 0, 0);
        }
    }

    void Activate(ID3D11DeviceContext *ctx)
    {
        ctx->IASetInputLayout(m_inputLayout.Get());

        ctx->VSSetShader(m_vs.Get(), nullptr, 0);
        if (m_vsConstants)
        {
            ID3D11Buffer *vsConstants[] = {m_vsConstants.Get()};
            ctx->VSSetConstantBuffers(0, _countof(vsConstants), vsConstants);
        }

        ctx->PSSetShader(m_ps.Get(), nullptr, 0);
        if (m_psConstants)
        {
            ID3D11Buffer *psConstants[] = {m_psConstants.Get()};
            ctx->PSSetConstantBuffers(0, _countof(psConstants), psConstants);
        }

        ctx->GSSetShader(nullptr, nullptr, 0);
    }
};

Shader::Shader()
    : m_impl(new ShaderImpl)
{
}

Shader::~Shader()
{
    delete m_impl;
}

static ComPtr<ID3DBlob> LoadCompileShader(const std::string &src, const char *name, const D3D_SHADER_MACRO *define, const char *target)
{
    UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

    ComPtr<ID3DBlob> ret;
    ComPtr<ID3DBlob> err;
    if (FAILED(D3DCompile(src.data(), src.size(), name, define, nullptr, "main", target, flags, 0, &ret, &err)))
    {
        auto error = (char *)err->GetBufferPointer();
        LOGE << name << ": " << error;
        LOGE << src;
        return nullptr;
    }
    return ret;
}

std::shared_ptr<Shader> Shader::Create(ID3D11Device *device, skeletal::scene::ShaderType shaderType)
{

    D3D_SHADER_MACRO vsMacro[] =
        {
            {
                .Name = "VERTEX_SHADER",
                .Definition = "1",
            },
            {0}};
    auto vsBlob = LoadCompileShader(g_unlit, "unlit.hlsl@vs", vsMacro, "vs_4_0");
    if (!vsBlob)
    {
        return nullptr;
    }
    ComPtr<ID3D11VertexShader> vs;
    if (FAILED(device->CreateVertexShader((DWORD *)vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &vs)))
    {
        return nullptr;
    }

    // create layout for vs
    D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}};
    ComPtr<ID3D11InputLayout> inputLayout;
    if (FAILED(device->CreateInputLayout(inputDesc, 2, vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &inputLayout)))
    {
        return nullptr;
    }

    D3D_SHADER_MACRO psMacro[] =
        {
            {
                .Name = "PIXEL_SHADER",
                .Definition = "1",
            },
            {0}};
    auto psBlob = LoadCompileShader(g_unlit, "unlit.hlsl@ps", psMacro, "ps_4_0");
    if (!psBlob)
    {
        return nullptr;
    }
    ComPtr<ID3D11PixelShader> ps;
    if (FAILED(device->CreatePixelShader((DWORD *)psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &ps)))
    {
        return nullptr;
    }

    D3D11_BUFFER_DESC desc = {0};
    desc.ByteWidth = sizeof(ConstantBuffer);
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    ComPtr<ID3D11Buffer> cb;
    if (FAILED(device->CreateBuffer(&desc, nullptr, &cb)))
    {
        return nullptr;
    }

    auto shader = std::shared_ptr<Shader>(new Shader);
    shader->m_impl->m_vs = vs;
    shader->m_impl->m_vsConstants = cb;
    shader->m_impl->m_inputLayout = inputLayout;
    shader->m_impl->m_ps = ps;
    shader->AttributeMap["POSITION"] = 0;
    shader->AttributeMap["NORMAL"] = 1;
    return shader;
}

void Shader::SetVSConstantBuffer(ID3D11DeviceContext *deviceContext, const ConstantBuffer &buffer)
{
    m_impl->SetVSConstantBuffer(deviceContext, buffer);
}

void Shader::Activate(ID3D11DeviceContext *deviceContext)
{
    m_impl->Activate(deviceContext);
}

} // namespace skeletal::dx11