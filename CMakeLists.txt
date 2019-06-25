CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)
PROJECT(Skeletal VERSION 0.1.0)

INCLUDE_DIRECTORIES($ENV{VCPKG_DIR}/installed/x64-windows/include)
LINK_DIRECTORIES($ENV{VCPKG_DIR}/installed/x64-windows/lib)
ADD_DEFINITIONS(
    -DUNICODE
    -D_UNICDOE
    )
ADD_COMPILE_OPTIONS(
    /std:c++latest 
    /EHsc
    )
SUBDIRS(SimpleGltfTests Skeletal)
