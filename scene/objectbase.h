#pragma once
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <simplegltf/simplegltf.h>

namespace agv
{
namespace scene
{

class ObjectBase
{
    uint32_t m_unique_id = -1;
    std::string m_name;

    // noncopyable
    ObjectBase() = default;
    ObjectBase(const ObjectBase &) = delete;
    ObjectBase &operator=(const ObjectBase &) = delete;

protected:
    ObjectBase(const std::string &name)
        : m_name(name)
    {
        static uint32_t s_next_unique_id = 1;
        m_unique_id = s_next_unique_id++;
    }

    virtual ~ObjectBase()
    {
    }

public:
    uint32_t GetID() const
    {
        return m_unique_id;
    }
    const std::string &GetName() const { return m_name; }
};

} // namespace scene
} // namespace agv
