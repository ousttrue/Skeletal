#pragma once

struct WindowState;

namespace skeletal::es3
{
class Renderer;
}

namespace skeletal::scene
{
class Scene;
}

namespace skeletal::gui
{
class GUIImpl;
class GUI
{
    GUIImpl *m_impl = nullptr;

public:
    GUI();
    ~GUI();

    void Draw(void *device, void *deviceContext,
        const WindowState *state, float deltaSeconds,
        skeletal::scene::Scene *scene);
};

} // namespace skeletal::gui