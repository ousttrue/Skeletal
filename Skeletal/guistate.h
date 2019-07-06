#pragma
#include <string>

namespace agv
{
namespace scene
{
class Scene;
}
namespace renderer
{
class GLES3Renderer;
}

} // namespace agv

struct GuiState
{
    bool loggerOpen = true;
    std::string logger;

    void Update(agv::scene::Scene *scene,
                agv::renderer::GLES3Renderer *renderer);
};
