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

    void Draw(const WindowState *state, float deltaSeconds,
               agv::scene::Scene *scene);
};

} // namespace gui
} // namespace agv