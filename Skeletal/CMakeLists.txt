SET(PROJECT_NAME Skeletal)

FILE(GLOB SRC
     *.cpp
     *.h
    ${CMAKE_CURRENT_LIST_DIR}/../SimpleGltf/simplegltf.h
    ) 
ADD_EXECUTABLE(${PROJECT_NAME} WIN32
    ${SRC}
    )
TARGET_LINK_LIBRARIES(${PROJECT_NAME}
    libEGL
    WinMM
    )
