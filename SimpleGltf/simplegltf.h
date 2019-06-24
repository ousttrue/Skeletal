#pragma once
#include <nlohmann/json.hpp>
#include <stdint.h>
#include <string_view>
#include <tuple>

namespace simple_gltf {

struct View {
  const std::byte *data = nullptr;
  size_t size = 0;

  View() : data(nullptr), size(0) {}

  View(const std::byte *data_, size_t size_) : data(data_), size(size_) {}

  std::tuple<View, View> read(size_t len) {
    return std::make_tuple(View(data + len, size - len),
                           View(data + len, size - len));
  }

  template <typename T> std::tuple<View, T> read() {
    return std::make_tuple(View(data + sizeof(T), size - sizeof(T)),
                           *(reinterpret_cast<const T *>(data)));
  }

  std::tuple<View, std::string_view> read_string(size_t len) {
    return std::make_tuple(
        View(data + len, size - len),
        std::string_view(reinterpret_cast<const char *>(data), len));
  }
};
static_assert(sizeof(View) == 16, "view size");

template <typename T>
static void find_and_set(const nlohmann::json &j, std::string_view key,
                         T &dst) {
  auto found = j.find(key);
  if (found != j.end()) {
    found->get_to(dst);
  }
}
#define json_get(json, obj, prop)                                              \
  { find_and_set(json, #prop, obj.prop); }
#define json_set(obj, prop)                                                    \
  { #prop, obj.prop }

///
/// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/schema/asset.schema.json
///
struct GltfAsset {
  std::string copy_write;
  std::string generator;
  std::string version;
  std::string minVersion;
};
void to_json(nlohmann::json &j, const GltfAsset &data) {
  j = nlohmann::json{{"copy_write", data.copy_write},
                     {"generator", data.generator},
                     {"version", data.version},
                     {"minVersion", data.minVersion}};
}
void from_json(const nlohmann::json &j, GltfAsset &data) {
  json_get(j, data, copy_write);
  json_get(j, data, generator);
  json_get(j, data, version);
  json_get(j, data, minVersion);
}

struct GltfChildOfRootProperty {
  std::string name;
};

///
/// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/schema/buffer.schema.json
///
struct GltfBuffer : GltfChildOfRootProperty {
  std::string uri;
  int byteLength = 0;
};
void to_json(nlohmann::json &j, const GltfBuffer &data) {
  j = nlohmann::json{
      {"name", data.name}, {"uri", data.uri}, {"byteLength", data.byteLength}};
}
void from_json(const nlohmann::json &j, GltfBuffer &data) {
  json_get(j, data, name);
  json_get(j, data, uri);
  json_get(j, data, byteLength);
}

///
/// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/schema/bufferView.schema.json
///
enum class GltfBufferViewTargetType {
  NONE = 0,
  ARRAY_BUFFER = 34962,
  ELEMENT_ARRAY_BUFFER = 34963,
};
struct GltfBufferView : GltfChildOfRootProperty {
  int buffer = -1;
  int byteOffset = 0;
  int byteLength = 0;
  int byteStride = 0;
  GltfBufferViewTargetType target = GltfBufferViewTargetType::NONE;
};
void to_json(nlohmann::json &j, const GltfBufferView &data) {
  j = nlohmann::json{json_set(data, name),       json_set(data, buffer),
                     json_set(data, byteOffset), json_set(data, byteLength),
                     json_set(data, byteStride), json_set(data, target)};
}
void from_json(const nlohmann::json &j, GltfBufferView &data) {
  json_get(j, data, name);
  json_get(j, data, buffer);
  json_get(j, data, byteOffset);
  json_get(j, data, byteLength);
  json_get(j, data, byteStride);
  json_get(j, data, target);
}

///
/// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md
///
struct Gltf {
  GltfAsset asset;
  std::vector<GltfBuffer> buffers;
  std::vector<GltfBufferView> bufferViews;
};
void to_json(nlohmann::json &j, const Gltf &data) {
  j = nlohmann::json{json_set(data, asset),
                     json_set(data, buffers, bufferViews)};
}
void from_json(const nlohmann::json &j, Gltf &data) {
  json_get(j, data, asset);
  json_get(j, data, buffers);
  json_get(j, data, bufferViews);
}

#undef json_get
#undef json_set

///
/// https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#glb-file-format-specification
///
struct Glb {
  std::string_view json;
  View bin;

  bool try_parse(View bytes) {
    std::string_view magic;
    std::tie(bytes, magic) = bytes.read_string(4);
    if (magic != "glTF") {
      return false;
    }

    uint32_t version;
    std::tie(bytes, version) = bytes.read<uint32_t>();
    if (version != 2) {
      return false;
    }

    uint32_t length;
    std::tie(bytes, length) = bytes.read<uint32_t>();

    {
      uint32_t chunk_length;
      std::tie(bytes, chunk_length) = bytes.read<uint32_t>();
      std::string_view chunk_type;
      std::tie(bytes, chunk_type) = bytes.read_string(4);
      if (chunk_type != "JSON") {
        return false;
      }
      std::tie(bytes, json) = bytes.read_string(chunk_length);
    }

    {
      uint32_t chunk_length;
      std::tie(bytes, chunk_length) = bytes.read<uint32_t>();
      std::string_view chunk_type;
      std::tie(bytes, chunk_type) = bytes.read_string(4);
      if (chunk_type != std::string_view("BIN\0", 4)) {
        return false;
      }
      std::tie(bytes, bin) = bytes.read(chunk_length);
    }

    return true;
  }
};

struct Storage {
  Gltf gltf;

  bool parse(const std::byte *bytes, size_t size, std::string_view path) {
    Glb glb;
    if (glb.try_parse(View(bytes, size))) {
      auto parsed = nlohmann::json::parse(glb.json);
      gltf = parsed.get<Gltf>();
      return true;
    } else {
      throw std::exception("not implemented");
    }
  }
};

} // namespace simple_gltf