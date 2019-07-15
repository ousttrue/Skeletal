#include "eglapp.h"
#include "win32_window.h"
#include <gles3renderer.h>
#include <scene.h>
#include <gui.h>
#include <plog/Log.h>

const auto WINDOW_NAME = L"Skeletal";

static std::wstring SjisToUnicode(const std::string &src)
{
    auto size = MultiByteToWideChar(CP_OEMCP, 0, src.c_str(), -1, NULL, 0);
    if (size == 0)
    {
        return L"";
    }
    std::vector<wchar_t> buf(size - 1);
    size = MultiByteToWideChar(CP_OEMCP, 0, src.c_str(), -1, &buf[0],
                               static_cast<int>(buf.size()));
    return std::wstring(buf.begin(), buf.end());
}

///
/// main
///
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    agv::gui::GUI gui;

    Win32Window window;
    if(!window.Create(CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_NAME))
    {
        return 1;
    }
    auto hwnd = (HWND)window.GetState().Handle;
    LOGD << "CreateWindow";

    EglApp app(hwnd);
    LOGD << "egl initialized";

    agv::scene::Scene scene;
    if (__argc == 1)
    {
        scene.CreateDefaultScene();
    }
    else
    {
        scene.Load(SjisToUnicode(__argv[1]));
    }

    agv::renderer::GLES3Renderer renderer;

    float lastTime = 0;
    while (window.IsRunning())
    {
        // update
        auto now = window.GetTimeSeconds();
        auto delta = now - lastTime;
        lastTime = now;
        scene.Update(now);

        auto state = window.GetState();

        // rendering
        gui.Draw(&state, delta, &renderer, &scene);
        app.present();
    }

    return 0;
}
