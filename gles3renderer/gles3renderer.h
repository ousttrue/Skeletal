#include <stdint.h>
#include <string>
#include <memory>
#include <scene.h>
#include <unordered_map>
#include "shadersourcemanager.h"

namespace agv
{
namespace renderer
{
struct GLES3RendererImpl;
class GLES3Renderer
{
    GLES3RendererImpl *m_impl;

public:
    GLES3Renderer();
    ~GLES3Renderer();

    // draw to render target, then return render target
    void Begin(const agv::scene::RenderTargetInfo *pInfo, agv::scene::Scene *pScene);
    void *End(const agv::scene::RenderTargetInfo *pInfo);

    void *GetTexture(uint32_t id) const;

private:
    void DrawNode(const agv::scene::RenderTargetInfo *pInfo, const agv::scene::Node *pNode);
    void DrawModel(const agv::scene::RenderTargetInfo *pInfo, const agv::scene::Model *pModel);
};
} // namespace renderer
} // namespace agv