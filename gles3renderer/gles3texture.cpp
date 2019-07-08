#include "gles3texture.h"
#define GL_GLEXT_PROTOTYPES
#include <GLES3/gl3.h>
#include <exception>
#include <array>

namespace agv
{
namespace renderer
{

GLES3Texture::GLES3Texture()
{
    glGenTextures(1, &m_texture);
}

GLES3Texture::~GLES3Texture()
{
    if (m_texture)
    {
        glDeleteTextures(1, &m_texture);
        m_texture = 0;
    }
}

void GLES3Texture::Bind(int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void GLES3Texture::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void GLES3Texture::SetImage(int width, int height, int channels, const std::byte *data)
{
    Bind(0);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    GLint format = 0;
    switch (channels)
    {
    case 3:
        format = GL_RGB;
        break;

    case 4:
        format = GL_RGBA;
        break;

    default:
        throw std::exception("invalid channels");
    }

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0,
                 format, GL_UNSIGNED_BYTE, static_cast<const void *>(data));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    Unbind();
}

//
GLES3RenderBuffer::GLES3RenderBuffer()
{
    glGenRenderbuffers(1, &m_renderBuffer);
}

GLES3RenderBuffer::~GLES3RenderBuffer()
{
    glDeleteRenderbuffers(1, &m_renderBuffer);
}

void GLES3RenderBuffer::ResizeDepth(int w, int h)
{
    Bind();
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
    Unbind();
}

void GLES3RenderBuffer::Bind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, m_renderBuffer);
}

void GLES3RenderBuffer::Unbind()
{
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

//
GLES3FrameBufferObject::GLES3FrameBufferObject()
{
    glGenFramebuffers(1, &m_fbo);

    m_depth = std::make_shared<GLES3RenderBuffer>();
    m_texture = std::make_shared<GLES3Texture>();
}

GLES3FrameBufferObject::~GLES3FrameBufferObject()
{
    glDeleteFramebuffers(1, &m_fbo);
}

void GLES3FrameBufferObject::Bind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

void GLES3FrameBufferObject::Unbind()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void GLES3FrameBufferObject::Resize(int width, int height)
{
    if (width == m_width && height == m_height)
    {
        return;
    }

    m_width = width;
    m_height = height;

    m_texture->SetImage(width, height, 4, nullptr);
    m_depth->ResizeDepth(width, height);

    glBindRenderbuffer(GL_RENDERBUFFER, m_depth->GetGLValue());
    Bind();
    int slot = 0;
    m_texture->Bind(slot);
    m_depth->Bind();
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, m_texture->GetGLValue(), slot);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, m_depth->GetGLValue());
    std::array<GLenum, 1> drawBuffers = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
    Unbind();
    m_texture->Unbind();
    m_depth->Unbind();
}

} // namespace renderer
} // namespace agv