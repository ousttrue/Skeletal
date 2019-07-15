#pragma once

struct WindowState;
namespace agv
{

namespace renderer
{
class GLES3Renderer;
}

namespace scene
{
class Scene;
}

namespace gui
{
class GUIImpl;
class GUI
{
    GUIImpl *m_impl = nullptr;

public:
    GUI();
    ~GUI();

    void Begin(const WindowState *state, float deltaSeconds,
               agv::renderer::GLES3Renderer *renderer,
               agv::scene::Scene *scene);
    void End();
};

} // namespace gui
} // namespace agv