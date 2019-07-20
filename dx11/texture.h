#pragma once

namespace skeletal::dx11
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

} // namespace skeletal::dx11
