#pragma once
#include <cstddef>

struct ID3D11Device;
struct ID3D11DeviceContext;
namespace skeletal::dx11
{

class TextureImpl;
class Texture
{
    TextureImpl *m_impl = nullptr;

public:
    Texture();
    ~Texture();
    void SetImage(ID3D11Device *device, int width, int height, int channels, const std::byte *data);
    void Activate(ID3D11DeviceContext *context, int slot);
    void* GetHandle();
};

} // namespace skeletal::dx11
