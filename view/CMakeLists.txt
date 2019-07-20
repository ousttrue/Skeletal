SET(SUB_NAME view)

FILE(GLOB SRC
	*.cpp
	*.h
	) 

ADD_LIBRARY(${SUB_NAME} 
    ${SRC}
    ${CMAKE_CURRENT_LIST_DIR}/../dependencies/im3d/im3d.cpp
	)

TARGET_COMPILE_DEFINITIONS(${SUB_NAME} PUBLIC
	IM3D_GIZMO_DEBUG
	)

TARGET_INCLUDE_DIRECTORIES(${SUB_NAME} PUBLIC
    ../dependencies/im3d/examples/common
	../dependencies/im3d
	../dx11
	)
