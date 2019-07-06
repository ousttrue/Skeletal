#pragma once
#include "objectbase.h"
#include "node_animation.h"
#include "dxm.h"

namespace agv
{
namespace scene
{

class MeshGroup;
class NodeValue : public ObjectBase
{
public:
    NodeValue(const std::string name) : ObjectBase(name)
    {
    }

    std::shared_ptr<MeshGroup> MeshGroup;
    std::shared_ptr<INodeAnimation> Animation;

};
typedef dxm::Tree<NodeValue> Node;

std::shared_ptr<Node> LoadNode(const simplegltf::Storage &storage,
          const simplegltf::GltfNode &gltfNode);

} // namespace scene
} // namespace agv
