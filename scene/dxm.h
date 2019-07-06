#pragma once
#include <DirectXMath.h>
#include <vector>
#include <memory>
#include <string>
#include <array>

// DirectXMath wrapper
namespace dxm
{
using namespace DirectX;

struct Vec3
{
    XMFLOAT3 Value;

    float *data() { return &Value.x; }
    const float *data() const { return &Value.x; }

    Vec3(float x, float y, float z)
        : Value(x, y, z)
    {
    }

    Vec3(const std::array<float, 3> &value)
        : Vec3(value[0], value[1], value[2])
    {
    }

    XMVECTOR Load() const
    {
        return XMLoadFloat3(&Value);
    }

    void Store(const XMVECTOR &value)
    {
        XMStoreFloat3(&Value, value);
    }

    static const Vec3 Zero;
    static const Vec3 One;
};
static_assert(sizeof(Vec3) == sizeof(XMFLOAT3));

struct Quaternion
{
    XMFLOAT4 Value;

    float *data() { return &Value.x; }
    const float *data() const { return &Value.x; }

    Quaternion(float x, float y, float z, float w)
        : Value(x, y, z, w)
    {
    }

    Quaternion(const std::array<float, 4> &value)
        : Quaternion(value[0], value[1], value[2], value[3])
    {
    }

    XMVECTOR Load() const
    {
        return XMLoadFloat4(&Value);
    }

    void Store(const XMVECTOR &value)
    {
        XMStoreFloat4(&Value, value);
    }

    static const Quaternion Identity;
};
static_assert(sizeof(Quaternion) == sizeof(XMFLOAT4));

struct Matrix
{
    XMFLOAT4X4 Value;

    float *data() { return &Value._11; }
    const float *data() const { return &Value._11; }

    Matrix(
        float _11, float _12, float _13, float _14,
        float _21, float _22, float _23, float _24,
        float _31, float _32, float _33, float _34,
        float _41, float _42, float _43, float _44)
        : Value(
              _11, _12, _13, _14,
              _21, _22, _23, _24,
              _31, _32, _33, _34,
              _41, _42, _43, _44)
    {
    }

    Matrix(const std::array<float, 16> &value)
        : Matrix(
              value[0], value[1], value[2], value[3],
              value[4], value[5], value[6], value[7],
              value[8], value[9], value[10], value[11],
              value[12], value[13], value[14], value[15])
    {
    }

    Matrix(const XMMATRIX &m)
    {
        Store(m);
    }

    Matrix(const Vec3 &s, const Quaternion &r, const Vec3 &t)
    {
        auto m = DirectX::XMMatrixTransformation(
            DirectX::XMVectorZero(), DirectX::XMQuaternionIdentity(), s.Load(),
            DirectX::XMVectorZero(), r.Load(),
            t.Load());
        Store(m);
    }

    XMMATRIX Load() const
    {
        return XMLoadFloat4x4(&Value);
    }

    Matrix &Store(const XMMATRIX &value)
    {
        XMStoreFloat4x4(&Value, value);
        return *this;
    }

    static const Matrix Identity;
};
static_assert(sizeof(Matrix) == sizeof(XMFLOAT4X4));

} // namespace dxm
