#include "node.h"
#include <plog/Log.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace agv
{
namespace scene
{

void Node::m_updateMatrix(const DirectX::XMMATRIX &parent)
{
    DirectX::XMMATRIX local;
    m_getLocalMatrix(&local);
    auto world = DirectX::XMMatrixMultiply(local, parent);
    DirectX::XMStoreFloat4x4(&m_worldMatrix, world);

    for (auto &child : m_children)
    {
        child->m_updateMatrix(world);
    }
}

void Node::m_getLocalMatrix(DirectX::XMMATRIX *pMatrix) const
{
    auto ScalingOrigin = DirectX::XMVectorZero();
    auto ScalingOrientationQuaternion = DirectX::XMQuaternionIdentity();
    auto Scaling = DirectX::XMLoadFloat3(&m_localScale);
    auto RotationOrigin = DirectX::XMVectorZero();
    auto RotationQuaternion = DirectX::XMLoadFloat4(&m_localRotation);
    auto Translation = DirectX::XMLoadFloat3(&m_localPosition);
    *pMatrix = DirectX::XMMatrixTransformation(
        ScalingOrigin, ScalingOrientationQuaternion, Scaling,
        RotationOrigin, RotationQuaternion,
        Translation);
}

DirectX::XMFLOAT4X4 Node::GetLocalMatrix() const
{
    DirectX::XMMATRIX m;
    m_getLocalMatrix(&m);

    DirectX::XMFLOAT4X4 ret;
    DirectX::XMStoreFloat4x4(&ret, m);
    return ret;
}

// DirectX::XMFLOAT3 Node::GetLocalEuler() const
// {

// }

void Node::SetLocalPosition(const DirectX::XMFLOAT3 &v)
{
    m_localPosition = v;
    UpdateMatrix();
}

void Node::SetLocalRotation(const DirectX::XMFLOAT4 &q)
{
    m_localRotation = q;
    UpdateMatrix();
}

void Node::SetLocalScale(const DirectX::XMFLOAT3 &s)
{
    m_localScale = s;
    UpdateMatrix();
}

void Node::SetLocalMatrix(const DirectX::XMFLOAT4X4 &m)
{
    auto vector = DirectX::XMLoadFloat4x4(&m);
    SetLocalMatrix(vector);
}

void Node::SetLocalMatrix(const DirectX::XMMATRIX &vector)
{
    // decompose
    DirectX::XMVECTOR s;
    DirectX::XMVECTOR r;
    DirectX::XMVECTOR t;
    if (DirectX::XMMatrixDecompose(&s, &r, &t, vector))
    {
        DirectX::XMStoreFloat3(&m_localScale, s);
        DirectX::XMStoreFloat4(&m_localRotation, r);
        DirectX::XMStoreFloat3(&m_localPosition, t);
        UpdateMatrix();
    }
}

void Node::SetWorldMatrix(const DirectX::XMFLOAT4X4 &m)
{
    auto parent = m_parent
                      ? DirectX::XMLoadFloat4x4(&m_parent->m_worldMatrix)
                      : DirectX::XMMatrixIdentity();
    if (m_parent)
    {
        auto i = 0;
    }
    // DirectX::XMVECTOR det;
    auto local = DirectX::XMMatrixMultiply(DirectX::XMLoadFloat4x4(&m), DirectX::XMMatrixInverse(nullptr, parent));
    SetLocalMatrix(local);
}

std::shared_ptr<Node> Node::Load(const simplegltf::Storage &storage,
                                 const simplegltf::GltfNode &gltfNode)
{
    auto &gltf = storage.gltf;
    auto node = std::make_shared<Node>(gltfNode.name);
    if (gltfNode.matrix.has_value())
    {
        node->SetLocalMatrix(*(DirectX::XMFLOAT4X4 *)&gltfNode.matrix.value());
    }
    else
    {
        if (gltfNode.translation.has_value())
        {
            node->SetLocalPosition(*(DirectX::XMFLOAT3 *)&gltfNode.translation.value());
        }
        if (gltfNode.rotation.has_value())
        {
            node->SetLocalRotation(*(DirectX::XMFLOAT4 *)&gltfNode.rotation.value());
        }
        if (gltfNode.scale.has_value())
        {
            node->SetLocalScale(*(DirectX::XMFLOAT3 *)&gltfNode.scale.value());
        }
    }
    return node;
}

} // namespace scene
} // namespace agv