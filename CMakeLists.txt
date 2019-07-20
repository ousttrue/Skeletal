CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)
PROJECT(Skeletal VERSION 0.1.0)

INCLUDE_DIRECTORIES(
    $ENV{VCPKG_DIR}/installed/x64-windows/include
    dependencies/simplegltf
    common
    )
LINK_DIRECTORIES($ENV{VCPKG_DIR}/installed/x64-windows/lib)
ADD_DEFINITIONS(
    -DUNICODE=1
    -D_UNICODE=1
    -DIM3D_VERTEX_ALIGNMENT=16
    )
ADD_COMPILE_OPTIONS(
    /std:c++latest 
    /EHsc
    )
SUBDIRS(common scene gui dx11 view Skeletal)
