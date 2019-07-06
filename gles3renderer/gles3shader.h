#include <stdint.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace agv
{
namespace renderer
{
class GLES3Shader
{
    uint32_t m_program = 0;

public:
    std::unordered_map<std::string, uint32_t> AttributeMap;
    GLES3Shader(uint32_t program);
    ~GLES3Shader();

    // scene::ShaderType
    static std::shared_ptr<GLES3Shader> Create(int shaderType);

    uint32_t GetUniformLocation(const std::string &name);
    void SetUniformValue(uint32_t location, const glm::mat4 &m);
    void SetUniformValue(const std::string &name, const glm::mat4 &m);
    void SetUniformValue(const std::string &name, int value);

    void Use();
};
} // namespace renderer
} // namespace agv