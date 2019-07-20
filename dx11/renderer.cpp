#include "renderer.h"

namespace skeletal::dx11
{

Renderer::Renderer()
{
}

Renderer::~Renderer()
{
}

void Renderer::Begin(const camera::CameraState *pInfo, skeletal::scene::Scene *pScene)
{
}

void *Renderer::End(const camera::CameraState *pInfo)
{
    return nullptr;
}

void *Renderer::GetTexture(uint32_t id) const
{
    return nullptr;
}

} // namespace skeletal::dx11