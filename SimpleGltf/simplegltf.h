#pragma once
#include <nlohmann/json.hpp>
#include <stdint.h>
#include <string_view>
#include <tuple>

namespace simple_gltf
{

struct View
{
    const std::byte *data = nullptr;
    size_t size = 0;

    View() : data(nullptr), size(0)
    {
    }

    View(const std::byte *data_, size_t size_)
        : data(data_), size(size_)
    {
    }

    std::tuple<View, View> read(size_t len)
    {
        return std::make_tuple(View(data + len, size - len),
                               View(data + len, size - len));
    }

    template <typename T>
    std::tuple<View, T> read()
    {
        return std::make_tuple(View(data + sizeof(T), size - sizeof(T)),
                               *(reinterpret_cast<const T *>(data)));
    }

    std::tuple<View, std::string_view> read_string(size_t len)
    {
        return std::make_tuple(View(data + len, size - len),
                               std::string_view(reinterpret_cast<const char *>(data), len));
    }
};
static_assert(sizeof(View) == 16, "view size");

template<typename T>
static void find_and_set(const nlohmann::json &j, std::string_view key, T &dst)
{
    auto found = j.find(key);
    if(found!=j.end()){
        found->get_to(dst);
    }
}
#define json_get(json, obj, prop) { find_and_set(json, #prop, obj.prop); }

///
/// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/schema/asset.schema.json
///
struct GltfAsset
{
    std::string copy_write;
    std::string generator;
    std::string version;
    std::string minVersion;
};
void to_json(nlohmann::json &j, const GltfAsset &data)
{
    j = nlohmann::json{
        {"copy_write", data.copy_write},
        {"generator", data.generator},
        {"version", data.version},
        {"minVersion", data.minVersion}};
}
void from_json(const nlohmann::json &j, GltfAsset &data)
{
    json_get(j, data, copy_write);
    json_get(j, data, generator);
    json_get(j, data, version);
    json_get(j, data, minVersion);
}

///
/// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md
///
struct Gltf
{
    GltfAsset asset;
};
void to_json(nlohmann::json &j, const Gltf &data)
{
    j = nlohmann::json{{"asset", data.asset}};
}
void from_json(const nlohmann::json &j, Gltf &data)
{
    // find_and_set(j, "asset", data.asset);
    json_get(j, data, asset);
}

#undef json_get

///
/// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#glb-file-format-specification
///
struct Glb
{
    std::string_view json;
    View bin;

    bool try_parse(View bytes)
    {
        std::string_view magic;
        std::tie(bytes, magic) = bytes.read_string(4);
        if (magic != "glTF")
        {
            return false;
        }

        uint32_t version;
        std::tie(bytes, version) = bytes.read<uint32_t>();
        if (version != 2)
        {
            return false;
        }

        uint32_t length;
        std::tie(bytes, length) = bytes.read<uint32_t>();

        {
            uint32_t chunk_length;
            std::tie(bytes, chunk_length) = bytes.read<uint32_t>();
            std::string_view chunk_type;
            std::tie(bytes, chunk_type) = bytes.read_string(4);
            if (chunk_type != "JSON")
            {
                return false;
            }
            std::tie(bytes, json) = bytes.read_string(chunk_length);
        }

        {
            uint32_t chunk_length;
            std::tie(bytes, chunk_length) = bytes.read<uint32_t>();
            std::string_view chunk_type;
            std::tie(bytes, chunk_type) = bytes.read_string(4);
            if (chunk_type != std::string_view("BIN\0", 4))
            {
                return false;
            }
            std::tie(bytes, bin) = bytes.read(chunk_length);
        }

        return true;
    }
};

struct Storage
{
    Gltf gltf;

    bool parse(const std::byte *bytes, size_t size, std::string_view path)
    {
        Glb glb;
        if (glb.try_parse(View(bytes, size)))
        {
            auto parsed = nlohmann::json::parse(glb.json);
            gltf = parsed.get<Gltf>();
            return true;
        }
        else
        {
            throw std::exception("not implemented");
        }
    }
};

} // namespace simple_gltf