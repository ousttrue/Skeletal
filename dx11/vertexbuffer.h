#pragma once
#include <memory>
#include <simplegltf/simplegltf.h>

struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
namespace skeletal::dx11
{

class VertexBufferGroupImpl;
class VertexBufferGroup
{
    VertexBufferGroupImpl *m_impl = nullptr;

public:
    VertexBufferGroupImpl *GetImpl() { return m_impl; }
    VertexBufferGroup(int vertexCount, simplegltf::GltfTopologyType topology);
    VertexBufferGroup(int vertexCount) : VertexBufferGroup(vertexCount, simplegltf::GltfTopologyType::TRIANGLES)
    {
    }
    ~VertexBufferGroup();
    void AddAttribute(ID3D11Device *device, const std::string &semantic, const simplegltf::View &view);
    void SetIndex(ID3D11Device *device, const simplegltf::View &view);
};

} // namespace skeletal::dx11