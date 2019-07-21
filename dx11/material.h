#pragma once
#include <memory>
#include <array>

struct ID3D11DeviceContext;
namespace skeletal::dx11
{

class Shader;
class Texture;
class Material
{
public:
    std::shared_ptr<Shader> Shader;

    std::array<float, 4> Color = {1, 1, 1, 1};

    std::shared_ptr<Texture> Texture;

    void Set(ID3D11DeviceContext *context);
};

} // namespace skeletal::dx11