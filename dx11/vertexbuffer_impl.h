#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <simplegltf/simplegltf.h>
#include <string>
#include <memory>
#include <unordered_map>

namespace skeletal::dx11
{

struct VertexBufferImpl
{
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;

    DXGI_FORMAT m_format;
    uint32_t GetStride()
    {
        switch (m_format)
        {
        case DXGI_FORMAT_R16_UINT:
            return 2;
        case DXGI_FORMAT_R32_UINT:
            return 4;
        case DXGI_FORMAT_R32G32B32_FLOAT:
            return 12;
        case DXGI_FORMAT_R32G32_FLOAT:
            return 8;
        default:
            throw std::exception("not implemented");
        }
    }

    void BindToIndex(ID3D11DeviceContext *context, UINT offset)
    {
        context->IASetIndexBuffer(m_buffer.Get(), m_format, offset);
    }
};

class VertexBufferGroupImpl
{
public:
    D3D11_PRIMITIVE_TOPOLOGY m_topology;
    std::unordered_map<std::string, std::shared_ptr<VertexBufferImpl>> m_attributes;
    std::shared_ptr<VertexBufferImpl> m_indices;

    void CreateVertexBuffer(ID3D11Device *device, const std::string &semantic, const simplegltf::View &view);
    void CreateIndexBuffer(ID3D11Device *device, const simplegltf::View &view);
};

} // namespace skeletal::dx11