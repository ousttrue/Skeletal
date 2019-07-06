#pragma once
#include <DirectXMath.h>

namespace agv
{
namespace scene
{
class ICamera
{
public:
    virtual const DirectX::XMFLOAT4X4 &GetMatrix() const = 0;
    virtual void SetScreenSize(int w, int h) = 0;
};

class PerspectiveCamera : public ICamera
{
    float m_fovYDegree = 30.0f;
    float m_aspect = 1.0f;
    float m_near = 0.05f;
    float m_far = 100.0f;

    DirectX::XMFLOAT4X4 m_matrix = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1};

public:
    PerspectiveCamera()
    {
        Calc();
    }

    void SetScreenSize(int w, int h)
    {
        m_aspect = w / (float)h;
        Calc();
    }

    void Calc()
    {
        auto m = DirectX::XMMatrixPerspectiveFovRH(DirectX::XMConvertToRadians(m_fovYDegree), m_aspect, m_near, m_far);
        DirectX::XMStoreFloat4x4(&m_matrix, m);
    }

    const DirectX::XMFLOAT4X4 &GetMatrix() const override
	{
		return m_matrix;
	}
};
} // namespace scene
} // namespace agv