#pragma once
#include <string>
#include <unordered_map>
#include <memory>

namespace skeletal::dx11
{

class Shader
{
    uint32_t m_program = 0;

public:
    std::unordered_map<std::string, uint32_t> AttributeMap;
    Shader(uint32_t program);
    ~Shader();

    // scene::ShaderType
    static std::shared_ptr<Shader> Create(int shaderType);

    uint32_t GetUniformLocation(const std::string &name);
    void SetUniformValue(uint32_t location, const float m[16]);
    void SetUniformValue(const std::string &name, const float m[16]);
    void SetUniformValue(const std::string &name, int value);

    void Use();
};

} // namespace skeletal::dx11