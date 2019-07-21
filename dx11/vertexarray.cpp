#include "vertexarray.h"
#include "vertexbuffer.h"
#include "vertexbuffer_impl.h"
#include "shader.h"

namespace skeletal::dx11
{
class VertexArrayImpl
{
    D3D11_PRIMITIVE_TOPOLOGY m_topology = (D3D11_PRIMITIVE_TOPOLOGY)0;
    std::shared_ptr<VertexBufferImpl> m_indices;
    std::vector<std::shared_ptr<VertexBufferImpl>> m_vertices;

    std::vector<ID3D11Buffer *> m_bufferPointers;
    std::vector<UINT> m_strides;
    std::vector<UINT> m_offsets;

public:
    VertexArrayImpl()
    {
    }

    void SetBuffer(const VertexBufferGroupImpl *buffer, const std::shared_ptr<Shader> &shader)
    {
        m_topology = buffer->m_topology;
        m_indices = buffer->m_indices;

        auto size = shader->AttributeMap.size();
        m_vertices.resize(size, nullptr);
        m_bufferPointers.resize(size);
        m_strides.resize(size, 0);
        m_offsets.resize(size, 0);

        for (auto kv : shader->AttributeMap)
        {
            auto found = buffer->m_attributes.find(kv.first);
            if (found != buffer->m_attributes.end())
            {
                auto vbo = found->second;
                auto index = kv.second;
                m_vertices[index] = vbo;
                m_bufferPointers[index] = vbo->m_buffer.Get();
                m_strides[index] = vbo->GetStride();
                m_offsets[index] = 0;
            }
        }
    }

    void Draw(ID3D11DeviceContext *context, int offset, int count)
    {
        context->IASetPrimitiveTopology(m_topology);
        context->IASetVertexBuffers(0, (UINT)m_bufferPointers.size(), m_bufferPointers.data(), m_strides.data(), m_offsets.data());
        m_indices->BindToIndex(context, offset);
        context->DrawIndexed(count, 0, 0);
    }
};

VertexArray::VertexArray()
    : m_impl(new VertexArrayImpl)
{
}

VertexArray::~VertexArray()
{
    delete m_impl;
}

void VertexArray::SetBuffer(const std::shared_ptr<VertexBufferGroup> &vbo, const std::shared_ptr<Shader> &shader)
{
    m_impl->SetBuffer(vbo->GetImpl(), shader);
}

void VertexArray::Draw(ID3D11DeviceContext *context, int offset, int count)
{
    m_impl->Draw(context, offset, count);
}

} // namespace skeletal::dx11