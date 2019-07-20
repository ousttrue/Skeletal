#include <stdint.h>
#include <string>
#include <memory>
#include <scene.h>
#include <unordered_map>
#include "shadersourcemanager.h"

namespace camera
{
struct CameraState;
}

namespace skeletal::es3
{

struct GLES3RendererImpl;
class GLES3Renderer
{
    GLES3RendererImpl *m_impl;

public:
    GLES3Renderer();
    ~GLES3Renderer();

    // draw to render target, then return render target
    void Begin(const camera::CameraState *pInfo, skeletal::scene::Scene *pScene);
    void *End(const camera::CameraState *pInfo);

    void *GetTexture(uint32_t id) const;

private:
    void DrawNode(const camera::CameraState *pInfo, const skeletal::scene::Node *pNode);
    void DrawModel(const camera::CameraState *pInfo, const skeletal::scene::Model *pModel);
};

} // namespace skeletal::es3