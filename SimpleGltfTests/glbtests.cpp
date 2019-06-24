#define CATCH_CONFIG_MAIN /
#include <catch2/catch.hpp>

#include "../SimpleGltf/simplegltf.h"
#include <fstream>
#include <string_view>
#include <vector>


auto read_allbytes(std::string_view path) -> decltype(auto) {
  std::vector<std::byte> data;

  // open the file for binary reading
  std::ifstream file;
  file.open(path.data(), std::ios_base::binary);
  if (file.is_open()) {
    // get the length of the file
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // read the file
    data.resize(fileSize);
    file.read(reinterpret_cast<char *>(&data[0]), fileSize);

    file.close();
  }

  return data;
}

auto ROOT_DIR = std::string("..\\..\\");

void test_gltf(const std::string &test_path) {
  auto path = ROOT_DIR + test_path;
  auto bytes = read_allbytes(path);
  simple_gltf::Storage storage;
  storage.parse(bytes.data(), bytes.size(), path);
  REQUIRE(!bytes.empty());
}

// TEST_CASE("SimpleMeshes", "[gltf]")
// {
//     test_gltf("dependencies\\glTF-Sample-Models\\2.0\\SimpleMeshes\\glTF\\SimpleMeshes.gltf");
// }

// TEST_CASE("key", "[json]")
// {
//     auto json = nlohmann::json{{}};
//     auto v = json.at("x");
//     auto a = 0;
// }

TEST_CASE("Cube", "[glb]") {
  test_gltf("dependencies\\glTF-Sample-Models\\2.0\\Box\\glTF-Binary\\Box.glb");
}
