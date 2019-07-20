#pragma
#include <string>

namespace skeletal::scene
{
class Scene;
}

namespace skeletal::dx11
{
class Renderer;
} // namespace skeletal::dx11

struct GuiState
{
    bool loggerOpen = true;
    bool assetsOpen = true;
    bool modelOpen = true;
    std::string logger;

    void Update(skeletal::scene::Scene *scene,
                skeletal::dx11::Renderer *renderer);
};
