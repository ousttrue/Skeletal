#pragma once
#include "objectbase.h"

namespace skeletal::scene
{

class Texture : public ObjectBase
{

public:
    std::string Mime;
    simplegltf::View Bytes;

    Texture(const std::string &name) : ObjectBase(name)
    {
    }

    void SetBytes(const std::string &mime, const simplegltf::View &view)
    {
        Mime = mime;
        Bytes = view;
    }

    static std::shared_ptr<Texture> Load(const simplegltf::Storage &storage, const simplegltf::GltfTexture &gltfTexture);
};

} // namespace skeletal::scene