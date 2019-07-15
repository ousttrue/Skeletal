#pragma once
#include "objectbase.h"
#include "dxm.h"
#include <plog/Log.h>

namespace agv
{
namespace scene
{

struct RenderTargetInfo
{
public:
    uint32_t CameraID = 0;
    dxm::Matrix Projection = dxm::Matrix::Identity;
    dxm::Matrix View = dxm::Matrix::Identity;
    dxm::Matrix View2 = dxm::Matrix::Identity;
    DirectX::XMINT4 Viewport = {0, 0, 100, 100};
    dxm::Vec4 ClearColor = dxm::Vec4::Zero;
    float ClearDepth = 1.0f;

    const dxm::Matrix &CalcMvp(const dxm::Matrix &model) const
    {
        // glm::mat4 mvp = projection * view * model;
        auto m = model.Load();
        auto v = View.Load();
        auto p = Projection.Load();
        m_mvp = DirectX::XMMatrixMultiply(DirectX::XMMatrixMultiply(m, v), p);
        return m_mvp;
    }

private:
    mutable dxm::Matrix m_mvp = dxm::Matrix::Identity;
};

class CameraBase : public ObjectBase
{
protected:
    CameraBase(const std::string &name) : ObjectBase(name)
    {
    }

public:
    virtual RenderTargetInfo &GetRenderTargetInfo() = 0;
    virtual const RenderTargetInfo &GetRenderTargetInfo() const = 0;
    virtual void SetViewPort(const DirectX::XMINT4 &viewport) = 0;
};

class PerspectiveCamera : public CameraBase
{
    float m_fovYDegree = 30.0f;
    float m_aspect = 1.0f;
    float m_near = 0.05f;
    float m_far = 100.0f;

    RenderTargetInfo m_info;

public:
    PerspectiveCamera(const std::string &name) : CameraBase(name)
    {
        m_info.CameraID = GetID();
        SetViewPort(DirectX::XMINT4(0, 0, 100, 100));
    }

    static std::shared_ptr<PerspectiveCamera> Create(const std::string &name)
    {
        auto camera = std::shared_ptr<PerspectiveCamera>(new PerspectiveCamera(name));
        return camera;
    }

    RenderTargetInfo &GetRenderTargetInfo() override
    {
        return m_info;
    }
    const RenderTargetInfo &GetRenderTargetInfo() const override
    {
        return m_info;
    }

    void SetViewPort(const DirectX::XMINT4 &viewport) override
    {
        if (viewport.z != m_info.Viewport.z || viewport.w != m_info.Viewport.w)
        {
            LOGD << "SetViewPort: " << viewport.z << ", " << viewport.w;
            m_aspect = (float)viewport.z / (float)viewport.w;
            Calc();
        }
        m_info.Viewport = viewport;
    }

    void Calc()
    {
        // m_info.Projection = DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(m_fovYDegree), m_aspect, m_near, m_far);

        const float ar = m_aspect;
        const float zNear = m_near;
        const float zFar = m_far;
        const float f = 1.0f / (float)tan(DirectX::XMConvertToRadians(m_fovYDegree * 0.5f));

        m_info.Projection.Value._11 = f / ar;
        m_info.Projection.Value._12 = 0.0f;
        m_info.Projection.Value._13 = 0.0f;
        m_info.Projection.Value._14 = 0.0f;

        m_info.Projection.Value._21 = 0.0f;
        m_info.Projection.Value._22 = f;
        m_info.Projection.Value._23 = 0.0f;
        m_info.Projection.Value._24 = 0.0f;

        m_info.Projection.Value._31 = 0.0f;
        m_info.Projection.Value._32 = 0.0f;
        m_info.Projection.Value._33 = (zNear + zFar) / (zNear - zFar);
        m_info.Projection.Value._34 = -1;

        m_info.Projection.Value._41 = 0.0f;
        m_info.Projection.Value._42 = 0.0f;
        m_info.Projection.Value._43 = (2 * zFar * zNear) / (zNear - zFar);
        m_info.Projection.Value._44 = 0.0f;
    }
};
} // namespace scene
} // namespace agv