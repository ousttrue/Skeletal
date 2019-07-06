#include "node.h"
#include <plog/Log.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/euler_angles.hpp>

namespace agv
{
namespace scene
{
using namespace dxm;

void Node::m_updateMatrix(const DirectX::XMMATRIX &parent)
{
    DirectX::XMMATRIX local;
    m_getLocalMatrix(&local);
    auto world = DirectX::XMMatrixMultiply(local, parent);
    m_worldMatrix = world;

    for (auto &child : m_children)
    {
        child->m_updateMatrix(world);
    }
}

Matrix Node::GetLocalMatrix() const
{
    DirectX::XMMATRIX m;
    m_getLocalMatrix(&m);
    return Matrix(m);
}

// DirectX::XMFLOAT3 Node::GetLocalEuler() const
// {

// }

void Node::SetLocalPosition(const Vec3 &v)
{
    m_localPosition = v;
    UpdateMatrix();
}

void Node::SetLocalRotation(const Quaternion &q)
{
    m_localRotation = q;
    UpdateMatrix();
}

void Node::SetLocalScale(const Vec3 &s)
{
    m_localScale = s;
    UpdateMatrix();
}

void Node::SetLocalMatrix(const DirectX::XMMATRIX &vector)
{
    // decompose
    DirectX::XMVECTOR s;
    DirectX::XMVECTOR r;
    DirectX::XMVECTOR t;
    if (DirectX::XMMatrixDecompose(&s, &r, &t, vector))
    {
        m_localScale.Store(s);
        m_localRotation.Store(r);
        m_localPosition.Store(t);
        UpdateMatrix();
    }
}

void Node::SetWorldMatrix(const Matrix &m)
{
    auto parent = GetParent();
    auto parentMatrix = parent
                            ? parent->m_worldMatrix.Load()
                            : DirectX::XMMatrixIdentity();
    // DirectX::XMVECTOR det;
    auto local = DirectX::XMMatrixMultiply(m.Load(), DirectX::XMMatrixInverse(nullptr, parentMatrix));
    SetLocalMatrix(local);
}

void Node::UpdateMatrix()
{
    if (auto parent = GetParent())
    {
        m_updateMatrix(parent->m_worldMatrix.Load());
    }
    else
    {
        m_updateMatrix(DirectX::XMMatrixIdentity());
    }
}

std::shared_ptr<Node> Node::Load(const simplegltf::Storage &storage,
                                 const simplegltf::GltfNode &gltfNode)
{
    auto &gltf = storage.gltf;
    auto node = std::shared_ptr<Node>(new Node(gltfNode.name));
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