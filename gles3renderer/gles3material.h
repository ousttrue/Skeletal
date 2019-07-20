#pragma once
#include <memory>
#include <glm/glm.hpp>

namespace skeletal::es3
{

class Shader;
class Texture;
class Material
{
public:
    std::shared_ptr<Shader> Shader;

    glm::vec4 Color = glm::vec4(1, 1, 1, 1);

    std::shared_ptr<Texture> Texture;

    void Set();
};

} // namespace skeletal::es3