SET(SUB_NAME gui)
SET(DEPENDENCIES ../dependencies)

FILE(GLOB SRC
	*.cpp
	*.h
    ${DEPENDENCIES}/imgui/imgui.cpp
    ${DEPENDENCIES}/imgui/imgui.h
    ${DEPENDENCIES}/imgui/imgui_widgets.cpp
    ${DEPENDENCIES}/imgui/imgui_widgets.h
    ${DEPENDENCIES}/imgui/imgui_demo.cpp
    ${DEPENDENCIES}/imgui/imgui_draw.cpp
    ${DEPENDENCIES}/imgui/imgui_internal.h
    ${DEPENDENCIES}/imgui/imconfig.h
    ${DEPENDENCIES}/imgui/stb_rect_pack.h
    ${DEPENDENCIES}/imgui/stb_textedit.h
    ${DEPENDENCIES}/imgui/stb_truetype.h
    ${DEPENDENCIES}/imgui/examples/imgui_impl_dx11.h
    ${DEPENDENCIES}/imgui/examples/imgui_impl_dx11.cpp
    ${DEPENDENCIES}/imgui/examples/imgui_impl_win32.h
    ${DEPENDENCIES}/imgui/examples/imgui_impl_win32.cpp
    ${DEPENDENCIES}/ImNodes/ImNodes.cpp
    ${DEPENDENCIES}/ImNodes/ImNodesEz.cpp
	) 

ADD_LIBRARY(${SUB_NAME} 
    ${SRC})

TARGET_COMPILE_DEFINITIONS(${SUB_NAME} PUBLIC
    IM3D_VERTEX_ALIGNMENT=16
	)

TARGET_INCLUDE_DIRECTORIES(${SUB_NAME} PUBLIC
    ../dependencies/imgui
    ../dependencies/ImNodes
    ../scene
    ../dx11
    ../view
	)
