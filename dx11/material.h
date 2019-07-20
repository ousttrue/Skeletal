#pragma once
#include <memory>
#include <array>

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

    void Set();
};

} // namespace skeletal::dx11