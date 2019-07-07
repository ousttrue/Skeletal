#include <stdint.h>
#include <string>
#include <memory>
#include <scene.h>
#include <unordered_map>
#include <DirectXMath.h>
#include "shadersourcemanager.h"

namespace agv
{
namespace renderer
{
struct GLES3RendererImpl;
class GLES3Renderer
{
    int m_width = 0;
    int m_height = 0;

    GLES3RendererImpl *m_impl;

public:
    GLES3Renderer();
    ~GLES3Renderer();
    void Resize(int w, int h);
    void Draw(agv::scene::Scene *pScene);

    void *GetOrCreateTexture(const agv::scene::Texture *pTexture);
    void *GetTexture(uint32_t id) const;
    DirectX::XMINT2 GetTextureSize(uint32_t id) const;

private:
    void DrawNode(const agv::scene::ICamera *camera, const agv::scene::Node *cameraNode, const agv::scene::Node *node);
    void DrawModel(const agv::scene::ICamera *camera, const agv::scene::Node *cameraNode, agv::scene::Model *pModel);
};
} // namespace renderer
} // namespace agv