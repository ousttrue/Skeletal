SET(PROJECT_NAME SimpleGltfTests)
FILE(GLOB SRC
     *.cpp
     *.h
    ) 
ADD_EXECUTABLE(${PROJECT_NAME} ${SRC})
