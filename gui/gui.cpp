#include "gui.h"
#include "window_state.h"
#include "guistate.h"
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <examples/imgui_impl_win32.h>
#include <exception>
#include <gles3renderer.h>
#include <scene.h>
#include <ImGuizmo.h>
#include <imgui_internal.h>

const char *glsl_version = "#version 300 es";

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

///
/// logger to imgui
///
namespace plog
{
template <class Formatter>
class ImGuiAppender : public IAppender
{
    std::string *m_buffer;

public:
    ImGuiAppender(std::string *buffer) : m_buffer(buffer), m_isatty(false) {}

    virtual void write(const Record &record)
    {
        util::nstring str = Formatter::format(record);
        util::MutexLock lock(m_mutex);

        for (auto c : str)
        {
            m_buffer->push_back(static_cast<char>(c));
        }
    }

protected:
    util::Mutex m_mutex;
    const bool m_isatty;
};
} // namespace plog

namespace agv
{
namespace gui
{
class GUIImpl
{
    void *m_hWnd = nullptr;
    bool m_openView = true;

    GuiState m_guiState;
    plog::ImGuiAppender<plog::TxtFormatter> m_appender;
    agv::renderer::GLES3Renderer m_renderer;

public:
    GUIImpl(void *hWnd)
        : m_hWnd(hWnd), m_appender(&m_guiState.logger)
    {
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        // (void)io;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
        io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;  // We can honor io.WantSetMousePos requests (optional, rarely used)

        //ImGui_ImplOpenGL3_CreateFontsTexture();
        ImGui_ImplOpenGL3_Init(glsl_version);
        ImGui_ImplWin32_Init(hWnd);

        plog::init(plog::verbose, &m_appender);
    }

    ~GUIImpl()
    {
        ImGui_ImplOpenGL3_Shutdown();
        ImGui::DestroyContext();
    }

    void Begin(const WindowState *state, float deltaSeconds, scene::Scene *scene)
    {
        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplWin32_NewFrame();

        ImGuiIO &io = ImGui::GetIO();
        IM_ASSERT(io.Fonts->IsBuilt());
        io.DeltaTime = deltaSeconds > 0 ? deltaSeconds : 0.001f;

        io.MouseDown[0] = state->Mouse.IsDown(ButtonFlags::Left);
        io.MouseDown[1] = state->Mouse.IsDown(ButtonFlags::Right);
        io.MouseDown[2] = state->Mouse.IsDown(ButtonFlags::Middle);
        io.MouseWheel = static_cast<float>(state->Mouse.Wheel);

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
            m_renderer.Begin(&info, scene);

            auto firstSelection = scene->m_selection.begin();
            if (m_openView && firstSelection != scene->m_selection.end())
            {
                auto selection = firstSelection->second;
                // gizmo
                // auto &info = scene->GetCamera()->GetRenderTargetInfo();
                {
                    auto model = selection->GetWorldMatrix();
                    info.CalcMvp(model);
                    // g_guizmo.ShowGuizmo(m_hWnd, ImVec2(0, 0), size, info.Projection, info.View, &model);
                    selection->SetWorldMatrix(model);
                }
            }

            auto result = m_renderer.End(&info);

            // show render target. vertical flip
            ImGui::Image(result, size, ImVec2(0, 1), ImVec2(1, 0));
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
                // ShowGui(&model);
                float matrixTranslation[3], matrixRotation[3], matrixScale[3];
                ImGuizmo::DecomposeMatrixToComponents(model.data(), matrixTranslation, matrixRotation, matrixScale);
                ImGui::InputFloat3("Tr", matrixTranslation, 3);
                ImGui::InputFloat3("Rt", matrixRotation, 3);
                ImGui::InputFloat3("Sc", matrixScale, 3);
                ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, model.data());
                selection->SetWorldMatrix(model);
            }
            ImGui::End();
        }

        m_guiState.Update(scene, &m_renderer);
    }

    void End()
    {
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
};

GUI::GUI()
{
}

GUI::~GUI()
{
    if (m_impl)
    {
        delete m_impl;
    }
}

void GUI::Draw(const WindowState *state, float deltaSeconds, agv::scene::Scene *scene)
{
    if (!m_impl)
    {
        m_impl = new GUIImpl(state->Handle);
    }
    m_impl->Begin(state, deltaSeconds, scene);
    m_impl->End();
}
} // namespace gui
} // namespace agv
