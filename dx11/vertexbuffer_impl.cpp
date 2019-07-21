#include "vertexbuffer_impl.h"
#include <plog/Log.h>
using namespace Microsoft::WRL;

namespace skeletal::dx11
{

void VertexBufferGroupImpl::CreateVertexBuffer(ID3D11Device *device, const std::string &semantic, const simplegltf::View &view)
{
    D3D11_BUFFER_DESC desc = {0};
    desc.ByteWidth = (UINT)view.size;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

    D3D11_SUBRESOURCE_DATA subRes = {0};
    subRes.pSysMem = view.data;

    ComPtr<ID3D11Buffer> buffer;
    if (FAILED(device->CreateBuffer(&desc, &subRes, &buffer)))
    {
        LOGE << "fail to create vertex buffer";
        return;
    }

    auto impl = std::make_shared<VertexBufferImpl>();
    impl->m_buffer = buffer;
    switch (view.valuetype)
    {
    case simplegltf::ValueType::FloatVec3:
        impl->m_format = DXGI_FORMAT_R32G32B32_FLOAT;
        break;

    case simplegltf::ValueType::FloatVec2:
        impl->m_format = DXGI_FORMAT_R32G32_FLOAT;
        break;

    default:
        throw std::exception("not implemented");
    }

    m_attributes.insert(std::make_pair(semantic, impl));
}

void VertexBufferGroupImpl::CreateIndexBuffer(ID3D11Device *device, const simplegltf::View &view)
{
    D3D11_BUFFER_DESC desc = {0};
    desc.ByteWidth = (UINT)view.size;
    desc.Usage = D3D11_USAGE_IMMUTABLE;
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

    D3D11_SUBRESOURCE_DATA subRes = {0};
    subRes.pSysMem = view.data;

    ComPtr<ID3D11Buffer> buffer;
    if (FAILED(device->CreateBuffer(&desc, &subRes, &buffer)))
    {
        LOGE << "fail to create index buffer";
        return;
    }

    m_indices = std::make_shared<VertexBufferImpl>();
    m_indices->m_buffer = buffer;
    switch (view.valuetype)
    {
    case simplegltf::ValueType::UInt16:
        m_indices->m_format = DXGI_FORMAT_R16_UINT;
        break;

    case simplegltf::ValueType::UInt32:
        m_indices->m_format = DXGI_FORMAT_R32_UINT;
        break;

    default:
        throw std::exception("not implemented");
    }
}

} // namespace skeletal::dx11