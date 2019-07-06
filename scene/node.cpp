#include "node.h"
#include <plog/Log.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace agv
{
namespace scene
{

std::shared_ptr<Node> LoadNode(const simplegltf::Storage &storage,
                               const simplegltf::GltfNode &gltfNode)
{
    auto &gltf = storage.gltf;
    auto node = std::make_shared<Node>(gltfNode.name);
    if (gltfNode.matrix.has_value())
    {
        node->SetLocalMatrix(dxm::Matrix(gltfNode.matrix.value()));
    }
    else
    {
        if (gltfNode.translation.has_value())
        {
            node->SetLocalPosition(dxm::Vec3(gltfNode.translation.value()));
        }
        if (gltfNode.rotation.has_value())
        {
            node->SetLocalRotation(dxm::Quaternion(gltfNode.rotation.value()));
        }
        if (gltfNode.scale.has_value())
        {
            node->SetLocalScale(dxm::Vec3(gltfNode.scale.value()));
        }
    }
    return node;
}

} // namespace scene
} // namespace agv