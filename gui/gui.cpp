#include "gui.h"
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_win32.h>
#include <exception>
#include <gles3renderer.h>
#include <scene.h>
#include <ImGuizmo.h>
#include <imgui_internal.h>

const char *glsl_version = "#version 300 es";

class Guizmo
{
    ImGuizmo::OPERATION mCurrentGizmoOperation = ImGuizmo::ROTATE;
    ImGuizmo::MODE mCurrentGizmoMode = ImGuizmo::LOCAL;
    bool useSnap = false;
    DirectX::XMFLOAT3 snap = {0, 0, 0};

    ImGuiContext *m_context = nullptr;

public:
    Guizmo()
    {
    }

    ~Guizmo()
    {
    }

    void ShowGui(dxm::Matrix *pM)
    {
        if (ImGui::IsKeyPressed(90))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
        if (ImGui::IsKeyPressed(69))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        if (ImGui::IsKeyPressed(82)) // r Key
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
            mCurrentGizmoOperation = ImGuizmo::TRANSLATE;

        ImGui::SameLine();
        if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
            mCurrentGizmoOperation = ImGuizmo::ROTATE;
        ImGui::SameLine();
        if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
            mCurrentGizmoOperation = ImGuizmo::SCALE;
        float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        ImGuizmo::DecomposeMatrixToComponents(pM->data(), matrixTranslation, matrixRotation, matrixScale);
        ImGui::InputFloat3("Tr", matrixTranslation, 3);
        ImGui::InputFloat3("Rt", matrixRotation, 3);
        ImGui::InputFloat3("Sc", matrixScale, 3);
        ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, pM->data());
        if (mCurrentGizmoOperation != ImGuizmo::SCALE)
        {
            if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
                mCurrentGizmoMode = ImGuizmo::LOCAL;
            ImGui::SameLine();
            if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
                mCurrentGizmoMode = ImGuizmo::WORLD;
        }
        if (ImGui::IsKeyPressed(83))
            useSnap = !useSnap;
        ImGui::Checkbox("", &useSnap);
        ImGui::SameLine();
        switch (mCurrentGizmoOperation)
        {
        case ImGuizmo::TRANSLATE:
            // snap = config.mSnapTranslation;
            ImGui::InputFloat3("Snap", &snap.x);
            break;
        case ImGuizmo::ROTATE:
            // snap = config.mSnapRotation;
            ImGui::InputFloat("Angle Snap", &snap.x);
            break;
        case ImGuizmo::SCALE:
            // snap = config.mSnapScale;
            ImGui::InputFloat("Scale Snap", &snap.x);
            break;
        }
    }

    void ShowGuizmo(HWND hWnd,
                    const ImVec2 &pos, const ImVec2 &size,
                    const dxm::Matrix &projection,
                    const dxm::Matrix &view,
                    dxm::Matrix *pM)
    {
        #if 1
        auto backup = ImGui::GetCurrentContext();
        {
            if (!m_context)
            {
                m_context = ImGui::CreateContext();
                ImGui::SetCurrentContext(m_context);
                ImGui_ImplOpenGL3_Init(glsl_version);

                // copy font
                m_context->IO.Fonts = backup->IO.Fonts;

                ImGui_ImplWin32_Init(hWnd);
            }
            ImGui::SetCurrentContext(m_context);

            // Start the Dear ImGui frame
            // ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplWin32_NewFrame();
            ImGui::NewFrame();
            {
                #endif

                ImGuizmo::BeginFrame();
                ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);
                auto m = dxm::Matrix::Identity;
                ImGuizmo::DrawGrid(view.data(), projection.data(), m.data(), 5);
                ImGuizmo::Manipulate(view.data(), projection.data(),
                                     mCurrentGizmoOperation, mCurrentGizmoMode, pM->data(), NULL,
                                     useSnap ? &snap.x : NULL);

                #if 1
            }
            ImGui::EndFrame();
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        }
        ImGui::SetCurrentContext(backup);
        #endif
    }
};
Guizmo g_guizmo;

namespace agv
{
namespace gui
{
GUI::GUI()
{
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;  // We can honor io.WantSetMousePos requests (optional, rarely used)
}

GUI::~GUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
}

void GUI::MouseMove(int x, int y)
{
    ImGuiIO &io = ImGui::GetIO();
    // Mouse position, in pixels (set to -1,-1 if no mouse / on another screen, etc.)
    io.MousePos = ImVec2(static_cast<float>(x), static_cast<float>(y));
}

void GUI::MouseLeftDown()
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[0] = true;
}

void GUI::MouseLeftUp()
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[0] = false;
}

void GUI::MouseMiddleDown()
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[2] = true;
}

void GUI::MouseMiddleUp()
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[2] = false;
}

void GUI::MouseRightDown()
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[1] = true;
}

void GUI::MouseRightUp()
{
    ImGuiIO &io = ImGui::GetIO();
    io.MouseDown[1] = false;
}

void GUI::MouseWheel(int d)
{
    ImGuiIO &io = ImGui::GetIO();
    if (d < 0)
    {
        io.MouseWheel -= 1;
    }
    else if (d > 0)
    {
        io.MouseWheel += 1;
    }
}

bool GUI::HasMouseCapture()
{
    ImGuiIO &io = ImGui::GetIO();
    return io.WantCaptureMouse;
}

bool GUI::HasFocus()
{
    ImGuiIO &io = ImGui::GetIO();
    return io.WantCaptureKeyboard;
}

bool GUI::IsHover()
{
    ImGuiIO &io = ImGui::GetIO();
    return ImGui::IsAnyWindowHovered() || io.WantCaptureMouse || io.WantCaptureKeyboard;
}

void GUI::SetScreenSize(int w, int h)
{
    ImGuiIO &io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)w, (float)h);
}

// Demonstrate using DockSpace() to create an explicit docking node within an existing window.
// Note that you already dock windows into each others _without_ a DockSpace() by just moving windows
// from their title bar (or by holding SHIFT if io.ConfigDockingWithShift is set).
// DockSpace() is only useful to construct to a central location for your application.
static void Dockspace()
{
    static bool opt_fullscreen_persistant = true;
    bool opt_fullscreen = opt_fullscreen_persistant;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
    // because it would be confusing to have two docking targets within each others.
    ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen)
    {
        ImGuiViewport *viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
        window_flags |= ImGuiWindowFlags_NoBackground;

    // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
    // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
    // all active windows docked into it will lose their parent and become undocked.
    // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
    // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("DockSpace Demo", nullptr, window_flags);
    ImGui::PopStyleVar();

    if (opt_fullscreen)
        ImGui::PopStyleVar(2);

    // DockSpace
    ImGuiIO &io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
    {
        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }
    else
    {
        throw std::exception("require ImGuiConfigFlags_DockingEnable");
        // ShowDockingDisabledMessage();
    }

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("Docking"))
        {
            // Disabling fullscreen would allow the window to be moved to the front of other windows,
            // which we can't undo at the moment without finer window depth/z control.
            //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

            if (ImGui::MenuItem("Flag: NoSplit", "", (dockspace_flags & ImGuiDockNodeFlags_NoSplit) != 0))
                dockspace_flags ^= ImGuiDockNodeFlags_NoSplit;
            if (ImGui::MenuItem("Flag: NoResize", "", (dockspace_flags & ImGuiDockNodeFlags_NoResize) != 0))
                dockspace_flags ^= ImGuiDockNodeFlags_NoResize;
            if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))
                dockspace_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
            if (ImGui::MenuItem("Flag: PassthruCentralNode", "", (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))
                dockspace_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
            if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (dockspace_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))
                dockspace_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
            ImGui::Separator();
            // if (ImGui::MenuItem("Close DockSpace", NULL, false, p_open != NULL))
            //     *p_open = false;
            ImGui::EndMenu();
        }
        /*
        HelpMarker(
            "When docking is enabled, you can ALWAYS dock MOST window into another! Try it now!"
            "\n\n"
            " > if io.ConfigDockingWithShift==false (default):"
            "\n"
            "   drag windows from title bar to dock"
            "\n"
            " > if io.ConfigDockingWithShift==true:"
            "\n"
            "   drag windows from anywhere and hold Shift to dock"
            "\n\n"
            "This demo app has nothing to do with it!"
            "\n\n"
            "This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window. This is useful so you can decorate your main application window (e.g. with a menu bar)."
            "\n\n"
            "ImGui::DockSpace() comes with one hard constraint: it needs to be submitted _before_ any window which may be docked into it. Therefore, if you use a dock spot as the central point of your application, you'll probably want it to be part of the very first window you are submitting to imgui every frame."
            "\n\n"
            "(NB: because of this constraint, the implicit \"Debug\" window can not be docked into an explicit DockSpace() node, because that window is submitted as part of the NewFrame() call. An easy workaround is that you can create your own implicit \"Debug##2\" window after calling DockSpace() and leave it in the window stack for anyone to use.)"
            );
        */

        ImGui::EndMenuBar();
    }

    ImGui::End();
}

void GUI::Begin(HWND hWnd, float deltaSeconds, agv::renderer::GLES3Renderer *renderer, agv::scene::Scene *scene)
{
    if (!m_initialized)
    {
        //ImGui_ImplOpenGL3_CreateFontsTexture();

        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui_ImplWin32_Init(hWnd);

        m_initialized = true;
    }

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();

    ImGuiIO &io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt());
    io.DeltaTime = deltaSeconds > 0 ? deltaSeconds : 0.001f;

    ImGui::NewFrame();

    // widgets...
    Dockspace();

    ImVec2 pos;
    ImVec2 size;
    auto camera = scene->GetCamera();
    auto &info = camera->GetRenderTargetInfo();

    // render centrarl wigets
    if (ImGui::Begin("3DView", &m_openView,
                     ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse))
    {
        // transfer MouseEvents
        auto &io = ImGui::GetIO();
        auto mouse = scene->GetMouseObserver();
        auto mousePos = ImGui::GetMousePos();
        mouse->MouseMove(static_cast<int>(mousePos.x), static_cast<int>(mousePos.y));
        if (io.MouseDown[1])
        {
            mouse->MouseRightDown();
        }
        if (io.MouseReleased[1])
        {
            mouse->MouseRightUp();
        }
        if (io.MouseDown[2])
        {
            mouse->MouseMiddleDown();
        }
        if (io.MouseReleased[2])
        {
            mouse->MouseMiddleUp();
        }
        mouse->MouseWheel(static_cast<int>(io.MouseWheel));

        // resize rendertarget
        pos = ImGui::GetWindowPos();
        size = ImGui::GetWindowSize();
        // size.y -= 40; // title bar ?
        camera->SetViewPort(DirectX::XMINT4(0, 0,
                                            static_cast<int>(size.x),
                                            static_cast<int>(size.y)));

        // render and get rendertarget
        renderer->Begin(&info, scene);

        auto firstSelection = scene->m_selection.begin();
        if (m_openView && firstSelection != scene->m_selection.end())
        {
            auto selection = firstSelection->second;
            // gizmo
            // auto &info = scene->GetCamera()->GetRenderTargetInfo();
            {
                auto model = selection->GetWorldMatrix();
                info.CalcMvp(model);
                g_guizmo.ShowGuizmo(hWnd, ImVec2(0, 0), size, info.Projection, info.View, &model);
                selection->SetWorldMatrix(model);
            }
        }

        auto result = renderer->End(&info);

        // show render target
        ImGui::Image(result, size);

        {
            auto cy = pos.y + size.y * 0.5f;
            auto &buf = ImGui::GetWindowDrawList()->VtxBuffer;
            for (int i = 0; i < buf.Size; i++)
                buf[i].pos.y += (cy - buf[i].pos.y) * 2;
        }
    }
    ImGui::End();

    auto firstSelection = scene->m_selection.begin();
    if (m_openView && firstSelection != scene->m_selection.end())
    {
        auto selection = firstSelection->second;
        // gizmo
        // auto &info = scene->GetCamera()->GetRenderTargetInfo();
        ImGuizmo::BeginFrame();
        if (ImGui::Begin("selected"))
        {
            auto model = selection->GetWorldMatrix();
            g_guizmo.ShowGui(&model);
            selection->SetWorldMatrix(model);
        }
        ImGui::End();
    }
}

void GUI::End()
{
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
} // namespace gui
} // namespace agv