#include "vertexbuffer.h"
#include "vertexbuffer_impl.h"
#include <plog/Log.h>
#include <d3d11.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;

namespace skeletal::dx11
{

VertexBufferGroup::VertexBufferGroup(int vertexCount, simplegltf::GltfTopologyType topology)
    : m_impl(new VertexBufferGroupImpl)
{
    switch (topology)
    {
    case simplegltf::GltfTopologyType::TRIANGLES:
        m_impl->m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        break;
    default:
        throw std::exception("not implemented");
    }
}

VertexBufferGroup::~VertexBufferGroup()
{
    delete m_impl;
}

void VertexBufferGroup::AddAttribute(ID3D11Device *device, const std::string &semantic, const simplegltf::View &view)
{
    if (semantic == "JOINTS_0" || semantic == "WEIGHTS_0")
    {
        // skip
        return;
    }
    m_impl->CreateVertexBuffer(device, semantic, view);
}

void VertexBufferGroup::SetIndex(ID3D11Device *device, const simplegltf::View &view)
{
    m_impl->CreateIndexBuffer(device, view);
}

} // namespace skeletal::dx11