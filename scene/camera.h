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
        auto m = DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(m_fovYDegree), m_aspect, m_near, m_far);
        m_info.Projection.Store(m);
    }
};
} // namespace scene
} // namespace agv