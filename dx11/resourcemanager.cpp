#include "resourcemanager.h"
#include "shader.h"
#include "texture.h"
#include "material.h"
#include "vertexbuffer.h"
#include "vertexarray.h"
#include <camera_state.h>
#include <scene.h>
#include <plog/Log.h>
#include <unordered_map>
#include <d3d11.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
struct StbImage
{
    unsigned char *data = nullptr;
    int width = 0;
    int height = 0;
    int channels = 0;

    int size() const
    {
        return width * height * channels;
    }

    bool Load(const std::byte *src, size_t size)
    {
        data = stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(src), static_cast<int>(size),
                                     &width, &height, &channels, 4);
        return data != 0;
    }

    ~StbImage()
    {
        if (data)
        {
            stbi_image_free(data);
            data = 0;
        }
    }
};

namespace skeletal::dx11
{

class ResourceManagerImpl
{
public:
    void Draw(ID3D11DeviceContext *deviceContext, const camera::CameraState *pInfo, skeletal::scene::Scene *pScene)
    {
        auto pModel = pScene->GetModel();
        for (auto &node : pModel->Nodes)
        {
            DrawNode(deviceContext, pInfo, &*node);
        }
    }

private:
    void DrawNode(ID3D11DeviceContext *deviceContext, const camera::CameraState *info, const skeletal::scene::Node *node)
    {
        auto context = (ID3D11DeviceContext *)deviceContext;
        ComPtr<ID3D11Device> d3d;
        context->GetDevice(&d3d);
        auto device = d3d.Get();

        auto &meshGroup = node->MeshGroup;
        if (meshGroup)
        {
            // node level constants
            ConstantBuffer nodeConst;
            nodeConst.ViewProjection = info->viewProjection;
            nodeConst.World = node->GetWorldMatrix().array();

            for (auto &mesh : meshGroup->Meshes)
            {
                auto vbo = GetOrCreateVertexBuffer(device, &*mesh);

                int offset = 0;
                for (auto &submesh : mesh->Submeshes)
                {
                    auto material = GetOrCreateMaterial(device, &*submesh->GetMaterial());
                    auto shader = material->Shader;
                    if (shader)
                    {
                        auto vao = GetOrCreateVertexArray(&*submesh, vbo, shader);
                        material->Set(deviceContext);

                        shader->SetVSConstantBuffer(deviceContext, nodeConst);

                        vao->Draw(deviceContext, offset, submesh->GetDrawCount());
                    }
                    offset += submesh->GetDrawCount();
                }
            }
        }
    }

    /// shader
    std::unordered_map<skeletal::scene::ShaderType, std::shared_ptr<Shader>> m_shader_map;
    std::shared_ptr<Shader> GetOrCreateShader(ID3D11Device *device, skeletal::scene::ShaderType shaderType)
    {
        auto found = m_shader_map.find(shaderType);
        if (found != m_shader_map.end())
        {
            return found->second;
        }

        auto shader = Shader::Create(device, shaderType);
        if (!shader)
        {
            LOGE << "fail to Shader::Create";
            return nullptr;
        }
        m_shader_map.insert(std::make_pair(shaderType, shader));

        return shader;
    }

    /// texture
    std::unordered_map<uint32_t, std::shared_ptr<Texture>> m_texture_map;
    void *GetTexture(uint32_t id) const
    {
        auto found = m_texture_map.find(id);
        if (found != m_texture_map.end())
        {
            return found->second->GetHandle();
        }
        return nullptr;
    }
    std::shared_ptr<Texture> GetOrCreateTexture(ID3D11Device *device, const skeletal::scene::Texture *pTexture)
    {
        auto found = m_texture_map.find(pTexture->GetID());
        if (found != m_texture_map.end())
        {
            return found->second;
        }

        auto texture = std::make_shared<Texture>();
        m_texture_map.insert(std::make_pair(pTexture->GetID(), texture));

        StbImage image;
        if (!image.Load(pTexture->Bytes.data, pTexture->Bytes.size))
        {
            LOGE << "fail to load image: " << pTexture->GetName();
            return nullptr;
        }

        texture->SetImage(device, image.width, image.height, image.channels, (const std::byte *)image.data);

        return texture;
    }

    /// material
    std::unordered_map<uint32_t, std::shared_ptr<Material>> m_material_map;
    std::shared_ptr<Material> GetOrCreateMaterial(ID3D11Device *device, const skeletal::scene::Material *pMaterial)
    {
        auto found = m_material_map.find(pMaterial->GetID());
        if (found != m_material_map.end())
        {
            return found->second;
        }

        auto material = std::make_shared<Material>();
        m_material_map.insert(std::make_pair(pMaterial->GetID(), material));

        material->Shader = GetOrCreateShader(device, pMaterial->ShaderType);

        // texture
        if (pMaterial->ColorTexture)
        {
            material->Texture = GetOrCreateTexture(device, &*pMaterial->ColorTexture);
        }

        return material;
    }

    /// vertex buffer
    std::unordered_map<uint32_t, std::shared_ptr<VertexBufferGroup>> m_vertexBuffer_map;
    std::shared_ptr<VertexBufferGroup> GetOrCreateVertexBuffer(ID3D11Device *device, const skeletal::scene::Mesh *pMesh)
    {
        {
            auto found = m_vertexBuffer_map.find(pMesh->GetID());
            if (found != m_vertexBuffer_map.end())
            {
                return found->second;
            }
        }

        auto vbo = std::make_shared<VertexBufferGroup>(pMesh->GetVertexCount(), pMesh->Topology);
        m_vertexBuffer_map.insert(std::make_pair(pMesh->GetID(), vbo));

        for (auto pair : pMesh->VertexAttributes)
        {
            vbo->AddAttribute(device, pair.first, pair.second);
        }

        auto &indices = pMesh->Indices;
        if (indices.data)
        {
            vbo->SetIndex(device, indices);
        }

        return vbo;
    }

    std::unordered_map<uint32_t, std::shared_ptr<VertexArray>> m_vertexArray_map;
    std::shared_ptr<VertexArray> GetOrCreateVertexArray(
        const skeletal::scene::Submesh *pSubmesh,
        const std::shared_ptr<VertexBufferGroup> &vbo,
        const std::shared_ptr<Shader> &shader)
    {
        {
            auto found = m_vertexArray_map.find(pSubmesh->GetID());
            if (found != m_vertexArray_map.end())
            {
                return found->second;
            }
        }

        auto vao = std::make_shared<VertexArray>();
        m_vertexArray_map.insert(std::make_pair(pSubmesh->GetID(), vao));

        vao->SetBuffer(vbo, shader);

        return vao;
    }
};

ResourceManager::ResourceManager()
    : m_impl(new ResourceManagerImpl)
{
}

ResourceManager::~ResourceManager()
{
    delete m_impl;
}

void ResourceManager::Draw(void *deviceContext, const camera::CameraState *pInfo, skeletal::scene::Scene *pScene)
{
    m_impl->Draw((ID3D11DeviceContext *)deviceContext, pInfo, pScene);
}

void *ResourceManager::GetTexture(uint32_t id) const
{
    return nullptr;
}

} // namespace skeletal::dx11