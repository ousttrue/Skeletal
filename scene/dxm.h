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

template <typename T>
struct Tree : std::enable_shared_from_this<Tree<T>>
{
    T m_value;

public:
    T &get() { return m_value; }
    const T &get() const { return m_value; }

    Tree(const std::string &name)
        : m_value(name)
    {
    }

private:
    std::weak_ptr<Tree> m_parent; // avoid cyclic reference

    typedef std::shared_ptr<Tree> TreePtr;
    std::vector<TreePtr> m_children;

public:
    TreePtr GetParent()
    {
        if (auto locked = m_parent.lock())
        {
            return locked;
        }
        else
        {
            return nullptr;
        }
    }

    int GetChildCount() const
    {
        return static_cast<int>(m_children.size());
    }

    typename std::vector<TreePtr>::iterator begin()
    {
        return m_children.begin();
    }

    typename std::vector<TreePtr>::iterator end()
    {
        return m_children.end();
    }

    const typename std::vector<TreePtr>::const_iterator begin() const
    {
        return m_children.begin();
    }

    const typename std::vector<TreePtr>::const_iterator end() const
    {
        return m_children.end();
    }

    void AddChild(const TreePtr &child)
    {
        assert(child && child.get() != this);
        child->m_parent = shared_from_this();
        m_children.push_back(child);
    }

private:
    // transformを SRT のローカル値で保持する
    Vec3 m_localPosition = Vec3::Zero;
    Quaternion m_localRotation = Quaternion::Identity;
    Vec3 m_localScale = Vec3::One;

    // 計算されたワールド値Vec3
    Matrix m_worldMatrix = Matrix::Identity;

    void m_updateMatrix(const DirectX::XMMATRIX &parent);
    void m_getLocalMatrix(DirectX::XMMATRIX *pLocal)
    {
        *pLocal = DirectX::XMMatrixTransformation(
            DirectX::XMVectorZero(), DirectX::XMQuaternionIdentity(), m_localScale.Load(),
            DirectX::XMVectorZero(), m_localRotation.Load(),
            m_localPosition.Load());
    }

public:
    Vec3 GetLocalPosition() const { return m_localPosition; }
    Quaternion GetLocalRotation() const { return m_localRotation; }
    Vec3 GetLocalScale() const { return m_localScale; }
    // DirectX::XMFLOAT3 GetLocalEuler() const;
    Matrix GetLocalMatrix() const;
    const Matrix &GetWorldMatrix() const { return m_worldMatrix; }
    void SetLocalPosition(const Vec3 &v);
    void SetLocalRotation(const Quaternion &q);
    void SetLocalScale(const Vec3 &s);
    void SetLocalMatrix(const Matrix &m)
    {
        SetLocalMatrix(m.Load());
    }
    void SetLocalMatrix(const DirectX::XMMATRIX &m);
    void SetWorldMatrix(const Matrix &m);
    void UpdateMatrix();
};

template <typename T>
void Tree<T>::m_updateMatrix(const DirectX::XMMATRIX &parent)
{
    DirectX::XMMATRIX local;
    m_getLocalMatrix(&local);
    auto world = DirectX::XMMatrixMultiply(local, parent);
    m_worldMatrix = world;

    for (auto &child : m_children)
    {
        child->m_updateMatrix(world);
    }
}

template <typename T>
Matrix Tree<T>::GetLocalMatrix() const
{
    DirectX::XMMATRIX m;
    m_getLocalMatrix(&m);

    auto ret = Matrix();
    ret.Store(m);
    return ret;
}

// DirectX::XMFLOAT3 Node::GetLocalEuler() const
// {

// }

template <typename T>
void Tree<T>::SetLocalPosition(const Vec3 &v)
{
    m_localPosition = v;
    UpdateMatrix();
}

template <typename T>
void Tree<T>::SetLocalRotation(const Quaternion &q)
{
    m_localRotation = q;
    UpdateMatrix();
}

template <typename T>
void Tree<T>::SetLocalScale(const Vec3 &s)
{
    m_localScale = s;
    UpdateMatrix();
}

template <typename T>
void Tree<T>::SetLocalMatrix(const DirectX::XMMATRIX &vector)
{
    // decompose
    DirectX::XMVECTOR s;
    DirectX::XMVECTOR r;
    DirectX::XMVECTOR t;
    if (DirectX::XMMatrixDecompose(&s, &r, &t, vector))
    {
        m_localScale.Store(s);
        m_localRotation.Store(r);
        m_localPosition.Store(t);
        UpdateMatrix();
    }
}

template <typename T>
void Tree<T>::SetWorldMatrix(const Matrix &m)
{
    auto parent = GetParent();
    auto parentMatrix = parent
                            ? parent->m_worldMatrix.Load()
                            : DirectX::XMMatrixIdentity();
    // DirectX::XMVECTOR det;
    auto local = DirectX::XMMatrixMultiply(m.Load(), DirectX::XMMatrixInverse(nullptr, parentMatrix));
    SetLocalMatrix(local);
}

template <typename T>
void Tree<T>::UpdateMatrix()
{
    if (auto parent = GetParent())
    {
        m_updateMatrix(parent->m_worldMatrix.Load());
    }
    else
    {
        m_updateMatrix(DirectX::XMMatrixIdentity());
    }
}

} // namespace dxm
