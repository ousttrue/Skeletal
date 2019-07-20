#pragma once
#include <cstdint>
#include <cstddef>
#include <memory>

namespace skeletal::es3
{

class Texture
{
    uint32_t m_texture = 0;

public:
    uint32_t GetGLValue() const { return m_texture; }
    Texture();
    ~Texture();
    void Bind(int slot);
    static void Unbind();
    void Texture::SetImage(int width, int height, int channels, const std::byte *data);
};

class RenderBuffer
{
    uint32_t m_renderBuffer;

public:
    RenderBuffer();
    ~RenderBuffer();
    void ResizeDepth(int w, int h);
    uint32_t GetGLValue() const { return m_renderBuffer; }
    void Bind();
    static void Unbind();
};

class FrameBufferObject
{
    uint32_t m_fbo = 0;
    std::shared_ptr<RenderBuffer> m_depth;
    std::shared_ptr<Texture> m_texture;
    int m_width = 0;
    int m_height = 0;

public:
    const std::shared_ptr<Texture> &GetTexture() const { return m_texture; }
    FrameBufferObject();
    ~FrameBufferObject();
    void Bind();
    static void Unbind();
    void Resize(int w, int h);
};

} // namespace skeletal::es3
