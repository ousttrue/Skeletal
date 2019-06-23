#define CATCH_CONFIG_MAIN /
#include <catch2/catch.hpp>

#include "../SimpleGltf/simplegltf.h"
#include <string_view>
#include <vector>
#include <fstream>

auto readAllBytes(const char *path) -> decltype(auto)
{
    std::vector<std::byte> data;

    // open the file for binary reading
    std::ifstream file;
    file.open(path, std::ios_base::binary);
    if (file.is_open())
    {
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

TEST_CASE("glb load", "[glb]")
{
    auto bytes = readAllBytes("..\\..\\dependencies\\glTF-Sample-Models\\2.0\\SimpleMeshes\\glTF\\SimpleMeshes.gltf");
    REQUIRE(!bytes.empty());
}
