#include <Windows.h>

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
class GUI
{
    bool m_initialized = false;
    bool m_openView = true;

public:
    GUI();
    ~GUI();

    void MouseMove(int x, int y);
    void MouseLeftDown();
    void MouseLeftUp();
    void MouseMiddleDown();
    void MouseMiddleUp();
    void MouseRightDown();
    void MouseRightUp();
    void MouseWheel(int d);

    bool HasMouseCapture();
    bool HasFocus();
    bool IsHover();

    void SetScreenSize(int w, int h);

    void Begin(HWND hWnd, float deltaSeconds,
               agv::renderer::GLES3Renderer *renderer,
               agv::scene::Scene *scene);
    void End();
};

} // namespace gui
} // namespace agv