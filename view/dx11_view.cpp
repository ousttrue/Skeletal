#include "window_state.h"

#include "dx11_view.h"
#include "dx11_renderer.h"
#include "orbit_camera.h"

#include "im3d_impl.h"
#include "im3d_impl_dx11.h"
#include <im3d.h>

#include <dx11.h>

class DX11ViewImpl
{
    OrbitCamera m_camera;
    std::array<float, 16> m_world = amth::IdentityMatrix();
    std::array<float, 4> m_view_color = {0.60f, 0.45f, 0.55f, 1.00f};
    DX11Renderer m_renderer;
    Im3dImplDx11 m_im3dImplDx11;

public:
    void *Draw(void *deviceContext, const WindowState &viewState,
               skeletal::scene::Scene *scene,
               skeletal::dx11::ResourceManager *resourceManager)
    {
        m_camera.WindowInput(viewState);

        UpdateGizmo(viewState);

        // setViewPort & clear background
        auto renderTarget = m_renderer.NewFrameToRenderTarget(deviceContext,
                                                            viewState.Width, viewState.Height, m_view_color.data());
        {
            // draw scene
            // DrawScene(deviceContext);
            resourceManager->Draw(deviceContext, &m_camera.state, scene);

            // draw gizmo
            m_im3dImplDx11.Draw(deviceContext, m_camera.state.viewProjection.data());
        }

        return renderTarget;
    }

    void DrawScene(void *deviceContext)
    {
        // use manipulated world
        m_renderer.DrawTeapot(deviceContext, m_camera.state.viewProjection.data(), m_world.data());
    }

    void UpdateGizmo(const WindowState &viewState)
    {
        //
        // gizmo update
        //
        Im3d_Impl_NewFrame(&m_camera.state, &viewState);
        // process gizmo, not draw, build draw list.
        Im3d::Gizmo("GizmoUnified", m_world.data());
        Im3d::EndFrame();
    }
};

DX11View::DX11View()
    : m_impl(new DX11ViewImpl)
{
}

DX11View::~DX11View()
{
    delete m_impl;
}

void *DX11View::Draw(void *deviceContext, const struct WindowState &viewState,
                     skeletal::scene::Scene *scene,
                     skeletal::dx11::ResourceManager *resourceManager)
{
    return m_impl->Draw(deviceContext, viewState, scene, resourceManager);
}
