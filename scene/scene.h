#pragma once
#include "mesh.h"
#include "node_animation.h"
#include "node.h"
#include "material.h"
#include "model.h"
#include "texture.h"

namespace skeletal::scene
{
class GLTFLoader;
class Scene
{
    float m_time = 0;
    uint32_t m_frameCount = 0;
    uint32_t m_seconds = 0;
    uint32_t m_fps = 0;

public:
    float GetTime() const { return m_time; }
    uint32_t GetFps() const { return m_fps; }

private:
    std::shared_ptr<Material> m_gizmoMaterial;

    std::shared_ptr<Model> m_model;

public:
    Model *GetModel() const
    {
        if (!m_model)
            return nullptr;
        return &*m_model;
    }

public:
    Scene();

private:
    std::vector<std::shared_ptr<Node>> m_gizmos;

public:
    int GetGizmosCount() const { return static_cast<int>(m_gizmos.size()); }
    const Node *GetGizmos(int index) const
    {
        if (index < 0 || index >= m_gizmos.size())
            return nullptr;
        return &*m_gizmos[index];
    }

public:
    void Setup();
    void Update(float now);
    void CreateDefaultScene();
    void Load(const std::wstring &path);

    std::unordered_map<uint32_t, std::shared_ptr<skeletal::scene::Node>> m_selection;

    void Select(const std::shared_ptr<Node> &node)
    {
        m_selection[node->GetID()] = node;
    }

    bool IsSelected(uint32_t id) const
    {
        return m_selection.find(id) != m_selection.end();
    }

    void ClearSelection()
    {
        m_selection.clear();
    }
};

} // namespace skeletal::scene
