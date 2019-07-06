#pragma once
#include "objectbase.h"
#include "animation_time.h"
#include "camera.h"
#include "node_animation.h"
#include <DirectXMath.h>

namespace agv
{
namespace scene
{

class MeshGroup;
class Node : public ObjectBase
{
public:
    Node(const std::string name) : ObjectBase(name)
    {
    }

    std::shared_ptr<MeshGroup> MeshGroup;
    std::shared_ptr<INodeAnimation> Animation;

private:
    Node *m_parent = nullptr;
    std::vector<std::shared_ptr<Node>> m_children;

public:
    Node *GetParent()
    {
        return m_parent;
    }

    std::vector<std::shared_ptr<Node>> &GetChildren()
    {
        return m_children;
    }

    void AddChild(const std::shared_ptr<Node> &child)
    {
        assert(!child->m_parent);
        child->m_parent = this;
        m_children.push_back(child);
    }

private:
    // transformを SRT のローカル値で保持する
    DirectX::XMFLOAT3 m_localPosition = {0, 0, 0};
    DirectX::XMFLOAT4 m_localRotation = {0, 0, 0, 1};
    DirectX::XMFLOAT3 m_localScale = {1, 1, 1};

    // 計算されたワールド値
    DirectX::XMFLOAT4X4 m_worldMatrix = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};

    void m_updateMatrix(const DirectX::XMMATRIX &parent);
    void m_getLocalMatrix(DirectX::XMMATRIX *pMatrix) const;

public:
    DirectX::XMFLOAT3 GetLocalPosition() const { return m_localPosition; }
    DirectX::XMFLOAT4 GetLocalRotation() const { return m_localRotation; }
    DirectX::XMFLOAT3 GetLocalScale() const { return m_localScale; }
    // DirectX::XMFLOAT3 GetLocalEuler() const;
    DirectX::XMFLOAT4X4 GetLocalMatrix() const;
    const DirectX::XMFLOAT4X4 &GetWorldMatrix() const { return m_worldMatrix; }
    void SetLocalPosition(const DirectX::XMFLOAT3 &v);
    void SetLocalRotation(const DirectX::XMFLOAT4 &q);
    void SetLocalScale(const DirectX::XMFLOAT3 &s);
    void SetLocalMatrix(const DirectX::XMFLOAT4X4 &m);
    void SetLocalMatrix(const DirectX::XMMATRIX &m);
    void UpdateMatrix()
    {
        if (m_parent)
        {
            m_updateMatrix(DirectX::XMLoadFloat4x4(&m_parent->m_worldMatrix));
        }
        else
        {
            m_updateMatrix(DirectX::XMMatrixIdentity());
        }
    }

public:
    static std::shared_ptr<Node> Load(const simplegltf::Storage &storage,
                                      const simplegltf::GltfNode &gltfNode);
};
} // namespace scene
} // namespace agv
