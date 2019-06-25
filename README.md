# Skeletal

## build

### install dependencies

* https://github.com/microsoft/vcpkg

* set environment variable `CMAKE_INSTALL_PREFIX=VCPKG_DIR/installed/x64-windows`
* vcpkg install catch2. `#include <catch2/catch.hpp>`
* vcpkg install nlohmann-json. `#include <nlohmann/json.hpp>`
* vcpkg install plog. `#include <plog/Log.h>`
* vcpkg install angle. `#include <EGL/egl.h>`
