#pragma once
#include "objectbase.h"
#include "shadertype.h"

namespace skeletal::scene
{

class Texture;
class Material : public ObjectBase
{
public:
    ShaderType ShaderType = ShaderType::gizmo;

    std::shared_ptr<Texture> ColorTexture;

    Material(const std::string &name) : ObjectBase(name)
    {
    }

    static std::shared_ptr<Material> Load(const simplegltf::Storage &storage,
                                          const simplegltf::GltfMaterial &material,
                                          const std::vector<std::shared_ptr<Texture>> &textures);
};

} // namespace skeletal::scene
