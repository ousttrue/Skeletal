#pragma once

namespace skeletal::scene
{
class Scene;
}

namespace skeletal::dx11
{
class ResourceManager;
} // namespace skeletal::dx11

class DX11ViewImpl;
class DX11View
{
    DX11ViewImpl *m_impl = nullptr;

public:
    DX11View();
    ~DX11View();
    void *Draw(void *deviceContext, const struct WindowState &viewState,
               skeletal::scene::Scene *scene,
               skeletal::dx11::ResourceManager *resourceManager);
};
