#include "material.h"
#include "shader.h"
#include "texture.h"

namespace skeletal::dx11
{

void Material::Set(ID3D11DeviceContext *context)
{
    // constant buffer
    Shader->Use(context);

    // ToDo: SetConstants

    Texture->Bind(context, 0);
}

} // namespace skeletal::dx11
