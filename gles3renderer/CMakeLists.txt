SET(SUB_NAME gles3renderer)

FILE(GLOB SRC
	*.cpp
	*.h
	) 

ADD_LIBRARY(${SUB_NAME} 
    ${SRC}
	${CMAKE_CURRENT_LIST_DIR}/../common/shadersourcemanager.cpp
	)

TARGET_COMPILE_DEFINITIONS(${SUB_NAME} PUBLIC
	UNICODE=1
	_UNICODE=1
	)

TARGET_INCLUDE_DIRECTORIES(${SUB_NAME} PUBLIC
    ../scene
    ../plog/include
    ../glm
	${CMAKE_INSTALL_PREFIX}/include
	)

