CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)
PROJECT(Skeletal VERSION 0.1.0)

INCLUDE_DIRECTORIES(
    $ENV{VCPKG_DIR}/installed/x64-windows/include
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/simplegltf
    # dependencies/imgui
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/imgui-node-editor/ThirdParty/imgui
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/imgui-node-editor/NodeEditor/Include
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/imgui-node-editor/NodeEditor/Source
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/imgui-node-editor/Examples/Common/BlueprintUtilities/Include
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/ImGuizmo
    ${CMAKE_CURRENT_LIST_DIR}/dependencies/imgui
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
