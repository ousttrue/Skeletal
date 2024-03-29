#include "dx11_context.h"
#include "win32_window.h"
#include <Windows.h>
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

    Win32Window window;
    auto hwnd = window.Create(CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_NAME);
    if (!hwnd)
    {
        return 1;
    }
    LOGD << "CreateWindow";

    DX11Context dx11;
    auto device = dx11.Create(hwnd);
    if (!device)
    {
        return 2;
    }
    LOGD << "dx11 initialized";
    auto deviceContext = dx11.GetDeviceContext();

    skeletal::scene::Scene scene;
    if (__argc == 1)
    {
        scene.CreateDefaultScene();
    }
    else
    {
        scene.Load(SjisToUnicode(__argv[1]));
    }

    skeletal::gui::GUI gui;
    while (true)
    {
        // update
        auto windowState = window.GetState();
        if (!windowState)
        {
            break;
        }
        scene.Update(windowState->DeltaSeconds);

        // create gui drawlist and draw 3D view to rendertarget
        gui.Update(hwnd, device, deviceContext, windowState, &scene);

        // get backbuffer
        dx11.NewFrame(windowState->Width, windowState->Height);
        // render to backbuffer
        gui.Render();
        // transfer backbuffer
        dx11.Present();
    }

    return 0;
}
