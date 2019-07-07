#include "texture.h"

namespace agv
{
namespace scene
{

std::shared_ptr<Texture> Texture::Load(const simplegltf::Storage &storage, const simplegltf::GltfTexture &gltfTexture)
{
    auto &gltf = storage.gltf;
    auto &image = gltf.images[gltfTexture.source];

    auto name = gltfTexture.name;
    if (name.empty())
    {
        name = image.name;
    }

    auto texture = std::make_shared<Texture>(name);

    texture->SetBytes(image.mimeType, storage.get_from_image(image));

    return texture;
}

std::shared_ptr<Texture> Texture::Create(const std::filesystem::path &path)
{
    auto texture = std::make_shared<Texture>(path.root_name().string());

    std::string mimeType;
    auto ext = path.extension();
    if (ext == ".png")
    {
        mimeType = "image/png";
    }
    else
    {
        return nullptr;
    }

    texture->SetBytes(mimeType, simplegltf::View::read_allbytes(path));

    return texture;
}

} // namespace scene
} // namespace agv