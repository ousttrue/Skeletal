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
    ../dx11
    ../scene
    ../gui
    ../dependencies/simplegltf
    )
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    common
    scene
    dx11
    gui
    WinMM
    d3d11
    )
