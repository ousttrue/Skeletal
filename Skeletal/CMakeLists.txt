SET(PROJECT_NAME Skeletal)

FILE(GLOB SRC
     *.cpp
     *.h
     *.rc
    ${CMAKE_CURRENT_LIST_DIR}/../SimpleGltf/simplegltf.h
    ) 
ADD_EXECUTABLE(${PROJECT_NAME} WIN32
    ${SRC}
    )
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME} PUBLIC
    ../gles3renderer
    ../scene
    ../gui
    ../dependencies/simplegltf
    )
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    scene
    gles3renderer
    gui
    WinMM
	debug 
	$ENV{VCPKG_DIR}/installed/x64-windows/debug/lib/libegl.lib
	$ENV{VCPKG_DIR}/installed/x64-windows/debug/lib/libGLESv2.lib
	optimized 
	$ENV{VCPKG_DIR}/installed/x64-windows/lib/libegl.lib
	$ENV{VCPKG_DIR}/installed/x64-windows/lib/libGLESv2.lib
    )
