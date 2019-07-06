#pragma once
#include "objectbase.h"
#include "node_animation.h"
#include "dxm.h"
#include <memory>

namespace agv
{
namespace scene
{

class MeshGroup;
class Node : public ObjectBase, public std::enable_shared_from_this<Node>
{
    Node(const std::string &name)
        : ObjectBase(name)
    {
    }

    std::weak_ptr<Node> m_parent; // avoid cyclic reference

    typedef std::shared_ptr<Node> NodePtr;
    std::vector<NodePtr> m_children;

public:
    static NodePtr Create(const std::string &name)
    {
        return std::shared_ptr<Node>(new Node(name));
    }

    NodePtr GetParent()
    {
        if (auto locked = m_parent.lock())
        {
            return locked;
        }
        else
        {
            return nullptr;
        }
    }

    int GetChildCount() const
    {
        return static_cast<int>(m_children.size());
    }

    typename std::vector<NodePtr>::iterator begin()
    {
        return m_children.begin();
    }

    typename std::vector<NodePtr>::iterator end()
    {
        return m_children.end();
    }

    const typename std::vector<NodePtr>::const_iterator begin() const
    {
        return m_children.begin();
    }

    const typename std::vector<NodePtr>::const_iterator end() const
    {
        return m_children.end();
    }

    void AddChild(const NodePtr &child)
    {
        assert(child && child.get() != this);
        auto shared = shared_from_this();
        child->m_parent = shared;
        m_children.push_back(child);
    }

private:
    // transformを SRT のローカル値で保持する
    dxm::Vec3 m_localPosition = dxm::Vec3::Zero;
    dxm::Quaternion m_localRotation = dxm::Quaternion::Identity;
    dxm::Vec3 m_localScale = dxm::Vec3::One;

    // 計算されたワールド値Vec3
    dxm::Matrix m_worldMatrix = dxm::Matrix::Identity;

    void m_updateMatrix(const DirectX::XMMATRIX &parent);
    void m_getLocalMatrix(DirectX::XMMATRIX *pLocal) const
    {
        *pLocal = DirectX::XMMatrixTransformation(
            DirectX::XMVectorZero(), DirectX::XMQuaternionIdentity(), m_localScale.Load(),
            DirectX::XMVectorZero(), m_localRotation.Load(),
            m_localPosition.Load());
    }

public:
    dxm::Vec3 GetLocalPosition() const { return m_localPosition; }
    dxm::Quaternion GetLocalRotation() const { return m_localRotation; }
    dxm::Vec3 GetLocalScale() const { return m_localScale; }
    // DirectX::XMFLOAT3 GetLocalEuler() const;
    dxm::Matrix GetLocalMatrix() const;
    const dxm::Matrix &GetWorldMatrix() const { return m_worldMatrix; }
    void SetLocalPosition(const dxm::Vec3 &v);
    void SetLocalRotation(const dxm::Quaternion &q);
    void SetLocalScale(const dxm::Vec3 &s);
    void SetLocalMatrix(const dxm::Matrix &m)
    {
        SetLocalMatrix(m.Load());
    }
    void SetLocalMatrix(const DirectX::XMMATRIX &m);
    void SetWorldMatrix(const dxm::Matrix &m);
    void UpdateMatrix();

    std::shared_ptr<MeshGroup> MeshGroup;
    std::shared_ptr<INodeAnimation> Animation;

    static std::shared_ptr<Node> Load(const simplegltf::Storage &storage,
                                      const simplegltf::GltfNode &gltfNode);
};

} // namespace scene
} // namespace agv
