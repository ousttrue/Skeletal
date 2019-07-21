#pragma once
#include <memory>
#include <array>

struct ID3D11Device;
struct ID3D11DeviceContext;
namespace skeletal::dx11
{

class Shader;
class Texture;
class MaterialImpl;
class Material
{
    MaterialImpl *m_impl = nullptr;

public:
    Material();
    ~Material();

    std::shared_ptr<Shader> Shader;

    std::array<float, 4> Color = {1, 1, 1, 1};

    std::shared_ptr<Texture> Texture;

    void Activate(ID3D11Device *device, ID3D11DeviceContext *context);
};

} // namespace skeletal::dx11