SET(PROJECT_NAME SimpleGltfTests)
FILE(GLOB SRC
     *.cpp
     *.h
    ${CMAKE_CURRENT_LIST_DIR}/../SimpleGltf/simplegltf.h
    ) 
ADD_EXECUTABLE(${PROJECT_NAME} 
    ${SRC}
    )
