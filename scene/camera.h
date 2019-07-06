#pragma once
#include <DirectXMath.h>

namespace agv
{
namespace scene
{
class ICamera
{
public:
    virtual void GetMatrix(DirectX::XMFLOAT4X4 *pM) const = 0;
    virtual void SetScreenSize(int w, int h) = 0;
};

class PerspectiveCamera : public ICamera
{
    float m_fovyDegree = 30.0f;
    float m_aspect = 1.0f;
    float m_near = 0.05f;
    float m_far = 100.0f;

public:
    PerspectiveCamera()
    {
    }

    void SetScreenSize(int w, int h)
    {
        m_aspect = w / (float)h;
    }

    void GetMatrix(DirectX::XMFLOAT4X4 *pM) const override
    {
        // return glm::perspective(glm::radians(m_fovyDegree), m_aspect, m_near, m_far);
		auto m = DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(m_fovyDegree), m_aspect, m_near, m_far);
		DirectX::XMStoreFloat4x4(pM, m);
    }
};
} // namespace scene
} // namespace agv