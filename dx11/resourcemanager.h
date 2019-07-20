#pragma once
#include <stdint.h>

namespace camera
{
struct CameraState;
}

namespace skeletal::scene
{
class Scene;
}

namespace skeletal::dx11
{

class ResourceManagerImpl;
class ResourceManager
{
    ResourceManagerImpl *m_impl = nullptr;

public:
    ResourceManager();
    ~ResourceManager();

    // draw to render target,
    void Begin(const camera::CameraState *pInfo, skeletal::scene::Scene *pScene);

    // then return render target
    void *End(const camera::CameraState *pInfo);

    void *GetTexture(uint32_t id) const;
};

} // namespace skeletal::dx11