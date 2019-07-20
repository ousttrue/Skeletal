#pragma once
#include <memory>
#include <vector>
#include <simplegltf/simplegltf.h>
#include "animation_time.h"
#include "dxm.h"

namespace skeletal::scene
{

class Texture;
class Material;
class MeshGroup;
class Node;
class Model
{
public:
    std::vector<std::shared_ptr<Texture>> Textures;
    std::vector<std::shared_ptr<Material>> Materials;
    std::vector<std::shared_ptr<MeshGroup>> Meshes;
    std::vector<std::shared_ptr<Node>> Nodes;
    const std::shared_ptr<Node> Root;

    Model();
    void SetTime(const AnimationTime &time);

    static std::shared_ptr<Model> Load(const simplegltf::Storage &storage);
};

} // namespace skeletal::scene