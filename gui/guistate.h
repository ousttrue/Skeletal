#pragma
#include <string>

namespace skeletal::scene
{
class Scene;

}

namespace skeletal::es3
{
class Renderer;

} // namespace skeletal::es3

struct GuiState
{
    bool loggerOpen = true;
    bool assetsOpen = true;
    bool modelOpen = true;
    std::string logger;

    void Update(skeletal::scene::Scene *scene,
                skeletal::es3::Renderer *renderer);
};
