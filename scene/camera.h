#pragma once
#include "dxm.h"

namespace agv
{
namespace scene
{
class ICamera
{
public:
    virtual const dxm::Matrix &GetMatrix() const = 0;
    virtual void SetScreenSize(int w, int h) = 0;
};

class PerspectiveCamera : public ICamera
{
    float m_fovYDegree = 30.0f;
    float m_aspect = 1.0f;
    float m_near = 0.05f;
    float m_far = 100.0f;

    dxm::Matrix m_matrix = dxm::Matrix::Identity;

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
        m_matrix.Store(m);
    }

    const dxm::Matrix &GetMatrix() const override
	{
		return m_matrix;
	}
};
} // namespace scene
} // namespace agv