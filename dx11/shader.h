#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <array>
#include <shadertype.h>

struct ID3D11Device;
struct ID3D11DeviceContext;

namespace skeletal::dx11
{

struct ConstantBuffer
{
    std::array<float, 16> World;
    std::array<float, 16> ViewProjection;
};

static_assert(sizeof(ConstantBuffer) == sizeof(float) * 16 * 2);
class ShaderImpl;
class Shader
{
    ShaderImpl *m_impl = nullptr;

public:
    std::unordered_map<std::string, uint32_t> AttributeMap;
    Shader();
    ~Shader();
    static std::shared_ptr<Shader> Create(ID3D11Device *device, skeletal::scene::ShaderType shaderType);
    void SetVSConstantBuffer(ID3D11DeviceContext *deviceContext, const ConstantBuffer &buffer);
    void Activate(ID3D11DeviceContext *deviceContext);
};

} // namespace skeletal::dx11