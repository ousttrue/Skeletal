#pragma once

struct WindowState;

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

    void Update(void *hwnd, void *device, void *deviceContext,
        const WindowState *windowState,
        skeletal::scene::Scene *scene);

    void Render();
};

} // namespace skeletal::gui