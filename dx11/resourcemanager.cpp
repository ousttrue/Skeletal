#include "resourcemanager.h"

namespace skeletal::dx11
{

ResourceManager::ResourceManager()
{
}

ResourceManager::~ResourceManager()
{
}

void ResourceManager::Begin(const camera::CameraState *pInfo, skeletal::scene::Scene *pScene)
{
}

void *ResourceManager::End(const camera::CameraState *pInfo)
{
    return nullptr;
}

void *ResourceManager::GetTexture(uint32_t id) const
{
    return nullptr;
}

} // namespace skeletal::dx11