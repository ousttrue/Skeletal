#include <Windows.h>

namespace agv::gui
{

class GUI
{
public:
    GUI();
    ~GUI();

    void Initialize(HWND hWnd);

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

    void Begin(float deltaSeconds);
    void End();
};

} // namespace agv::gui