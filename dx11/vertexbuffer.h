#pragma once
#include <memory>
#include <simplegltf/simplegltf.h>

namespace skeletal::dx11
{

class VertexBuffer
{
    bool m_isIndex = false;
    int m_componentCount = 0;

public:
    VertexBuffer();
    ~VertexBuffer();
    void Bind();
    void Unbind();
    static void UnbindIndex();
    static void UnbindAttribute();
    void BufferData(bool isIndex, const std::byte *values, size_t byteSize, int componentCount);
    int GetComponentCount() const { return m_componentCount; }
};

///
/// scene Mesh から作られるVBOをグループ化する
///
class VertexBufferGroup
{
public:
    std::unordered_map<std::string, std::shared_ptr<VertexBuffer>> m_attributes;
    std::shared_ptr<VertexBuffer> m_indices;

private:
    uint32_t m_topology;
    int m_vertexCount;

    int m_indexCount;
    uint32_t m_indexType;

public:
    VertexBufferGroup(int vertexCount, simplegltf::GltfTopologyType topology);
    VertexBufferGroup(int vertexCount) : VertexBufferGroup(vertexCount, simplegltf::GltfTopologyType::TRIANGLES)
    {
    }
    void AddAttribute(const std::string &semantic, const simplegltf::View &view);
    void SetIndex(const simplegltf::View &view);
    void Draw(int offset, int count);
};


class VertexArrayImpl;
class VertexArray
{
    VertexArrayImpl *m_impl = nullptr;

public:
    VertexArray();
    ~VertexArray();
    void BindSlot(int slot, const std::shared_ptr<VertexBuffer> &vbo);
    void Bind();
    static void Unbind();
};

} // namespace skeletal::dx11