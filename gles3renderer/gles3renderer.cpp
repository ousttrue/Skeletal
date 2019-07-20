﻿#include "gles3renderer.h"
#include "gles3vertexbuffer.h"
#include "gles3material.h"
#include "gles3shader.h"
#include "gles3texture.h"
#include "scene.h"
#include <camera_state.h>
#define GL_GLEXT_PROTOTYPES
#include <GLES3/gl3.h>
#include <vector>
#include <exception>
#include <plog/Log.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

namespace skeletal::es3
{

///
/// resource manager
///
struct RendererImpl
{
    /// shader
    std::unordered_map<skeletal::scene::ShaderType, std::shared_ptr<Shader>> m_shader_map;
    std::shared_ptr<Shader> GetOrCreateShader(skeletal::scene::ShaderType shaderType)
    {
        auto found = m_shader_map.find(shaderType);
        if (found != m_shader_map.end())
        {
            return found->second;
        }

        auto shader = Shader::Create((int)shaderType);
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
            return (void *)(int64_t)found->second->GetGLValue();
        }
        return nullptr;
    }
    std::shared_ptr<Texture> GetOrCreateTexture(const skeletal::scene::Texture *pTexture)
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

        texture->SetImage(image.width, image.height, image.channels, (const std::byte *)image.data);

        return texture;
    }

    /// material
    std::unordered_map<uint32_t, std::shared_ptr<Material>> m_material_map;
    std::shared_ptr<Material> GetOrCreateMaterial(const skeletal::scene::Material *pMaterial)
    {
        auto found = m_material_map.find(pMaterial->GetID());
        if (found != m_material_map.end())
        {
            return found->second;
        }

        auto material = std::make_shared<Material>();
        m_material_map.insert(std::make_pair(pMaterial->GetID(), material));

        material->Shader = GetOrCreateShader(pMaterial->ShaderType);

        // texture
        if (pMaterial->ColorTexture)
        {
            material->Texture = GetOrCreateTexture(&*pMaterial->ColorTexture);
        }

        return material;
    }

    /// vertex buffer
    std::unordered_map<uint32_t, std::shared_ptr<VertexBufferGroup>> m_vertexBuffer_map;
    std::shared_ptr<VertexBufferGroup> GetOrCreateVertexBuffer(const skeletal::scene::Mesh *pMesh)
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
            vbo->AddAttribute(pair.first, pair.second);
        }

        auto &indices = pMesh->Indices;
        if (indices.data)
        {
            vbo->SetIndex(indices);
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

        vao->Bind();

        for (auto kv : shader->AttributeMap)
        {
            auto found = vbo->m_attributes.find(kv.first);
            if (found != vbo->m_attributes.end())
            {
                found->second->Bind();
                vao->BindSlot(kv.second, found->second);
            }
        }
        if (vbo->m_indices)
        {
            vbo->m_indices->Bind();
        }

        VertexArray::Unbind();
        VertexBuffer::UnbindIndex();
        VertexBuffer::UnbindAttribute();

        return vao;
    }

    std::unordered_map<uint32_t, std::shared_ptr<FrameBufferObject>> m_camera_map;
    std::shared_ptr<FrameBufferObject> GetOrCreateCamera(
        uint32_t cameraID, int width, int height)
    {
        auto found = m_camera_map.find(cameraID);
        std::shared_ptr<FrameBufferObject> fbo;
        if (found != m_camera_map.end())
        {
            fbo = found->second;
        }
        else
        {
            fbo = std::make_shared<FrameBufferObject>();
            m_camera_map.insert(std::make_pair(cameraID, fbo));
        }

        fbo->Resize(width, height);

        return fbo;
    }
};

Renderer::Renderer()
    : m_impl(new RendererImpl)
{
}

Renderer::~Renderer()
{
    delete m_impl;
}

void Renderer::DrawNode(const camera::CameraState *info, const skeletal::scene::Node *node)
{
    auto &meshGroup = node->MeshGroup;
    if (meshGroup)
    {
        for (auto &mesh : meshGroup->Meshes)
        {
            auto vbo = m_impl->GetOrCreateVertexBuffer(&*mesh);

            int offset = 0;
            for (auto &submesh : mesh->Submeshes)
            {
                auto material = m_impl->GetOrCreateMaterial(&*submesh->GetMaterial());
                auto shader = material->Shader;
                if (shader)
                {
                    shader->Use();

                    shader->SetUniformValue("ProjectionMatrix", info->projection.data());

                    shader->SetUniformValue("ViewMatrix", info->view.data());

                    auto model = node->GetWorldMatrix().array();
                    shader->SetUniformValue("ModelMatrix", model.data());

                    shader->SetUniformValue("MVPMatrix", info->CalcModelViewProjection(model).data());

                    // set texture
                    material->Set();

                    auto vao = m_impl->GetOrCreateVertexArray(&*submesh, vbo, shader);
                    vao->Bind();
                    vbo->Draw(offset, submesh->GetDrawCount());
                }
                offset += submesh->GetDrawCount();
            }
            VertexArray::Unbind();
        }
    }
}

void Renderer::Begin(const camera::CameraState *pInfo, skeletal::scene::Scene *pScene)
{
    // auto &vp = pInfo->Viewport;
    // auto &clear = pInfo->ClearColor.Value;

    auto camera = m_impl->GetOrCreateCamera(pInfo->UserDataAsUInt(),
                                            pInfo->viewportWidth, pInfo->viewportHeight);
    camera->Bind();

    //
    // rendertarget
    //
    glViewport(0, 0, pInfo->viewportWidth, pInfo->viewportHeight);
    glClearColor(pInfo->clearColor[0], pInfo->clearColor[1], pInfo->clearColor[2], pInfo->clearColor[3]);
    glClearDepthf(pInfo->clearDepth);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    // Draw
    //
    if (pScene)
    {
        {
            auto count = pScene->GetGizmosCount();
            for (int i = 0; i < count; ++i)
            {
                auto node = pScene->GetGizmos(i);
                DrawNode(pInfo, node);
            }
        }

        DrawModel(pInfo, pScene->GetModel());
    }
}

void *Renderer::End(const camera::CameraState *pInfo)
{
    auto camera = m_impl->GetOrCreateCamera(pInfo->UserDataAsUInt(),
                                            pInfo->viewportWidth, pInfo->viewportHeight);
    camera->Unbind();
    return (void *)(int64_t)camera->GetTexture()->GetGLValue();
}

void Renderer::DrawModel(const camera::CameraState *pInfo,
                              const skeletal::scene::Model *pModel)
{
    for (auto &node : pModel->Nodes)
    {
        DrawNode(pInfo, &*node);
    }
}

void *Renderer::GetTexture(uint32_t id) const
{
    return m_impl->GetTexture(id);
}

} // namespace skeletal::es3