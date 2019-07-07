CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)
PROJECT(Skeletal VERSION 0.1.0)

INCLUDE_DIRECTORIES(
    $ENV{VCPKG_DIR}/installed/x64-windows/include
    dependencies/simplegltf
    dependencies/imgui
    dependencies/ImGuizmo
    dependencies/imgui-node-editor/NodeEditor/Include
    )
LINK_DIRECTORIES($ENV{VCPKG_DIR}/installed/x64-windows/lib)
ADD_DEFINITIONS(
    -DUNICODE
    -D_UNICDOE
    )
ADD_COMPILE_OPTIONS(
    /std:c++latest 
    /EHsc
    )
SUBDIRS(gles3renderer scene gui Skeletal)
