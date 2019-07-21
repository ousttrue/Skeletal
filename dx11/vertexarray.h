#pragma once
#include "vertexbuffer.h"
#include <memory>
#include <d3d11.h>

struct ID3D11DeviceContext;
namespace skeletal::dx11
{

class VertexBuffer;
class Shader;
class VertexArrayImpl;
class VertexArray
{
    VertexArrayImpl *m_impl = nullptr;

public:
    VertexArray();
    ~VertexArray();
    void SetBuffer(const std::shared_ptr<VertexBufferGroup> &vbo,
                   const std::shared_ptr<Shader> &shader);
    void Draw(ID3D11DeviceContext *context, int offset, int count);
};

} // namespace skeletal::dx11