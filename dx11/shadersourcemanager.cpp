#include "shadersourcemanager.h"
#include "material.h"
#include <plog/Log.h>
#include <windows.h>

// ///
// /// shader
// ///
// const std::string g_gizmo_vs =
// #include "../shaders/gizmo.vs"
//     ;
// const std::string g_gizmo_fs =
// #include "../shaders/gizmo.fs"
//     ;
// const std::string g_unlit_vs =
// #include "../shaders/unlit.vs"
//     ;
// const std::string g_unlit_fs =
// #include "../shaders/unlit.fs"
//     ;

static std::string trim(const std::string &src)
{
    auto it = src.begin();
    for (; it != src.end(); ++it)
    {
        if (!isspace(*it))
        {
            break;
        }
    }
    return std::string(it, src.end());
}

namespace skeletal
{

ShaderSourceManager ShaderSourceManager::Instance;

ShaderSourceManager::ShaderSourceManager()
{
    // skeletal::ShaderSourceManager::Instance.SetSource(
    //     skeletal::scene::ShaderType::gizmo,
    //     trim(g_gizmo_vs),
    //     trim(g_gizmo_fs));
    // skeletal::ShaderSourceManager::Instance.SetSource(
    //     skeletal::scene::ShaderType::unlit,
    //     trim(g_unlit_vs),
    //     trim(g_unlit_fs));
}

std::shared_ptr<ShaderSource> ShaderSourceManager::GetSource(scene::ShaderType shaderType)
{
    auto found = m_sourceMap.find(shaderType);
    if (found == m_sourceMap.end())
    {
        LOGE << "shader not found";
        return nullptr;
    }
    return found->second;
}

} // namespace skeletal