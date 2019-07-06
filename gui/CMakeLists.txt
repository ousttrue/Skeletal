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
    ${DEPENDENCIES}/imgui/examples/imgui_impl_opengl3.h
    ${DEPENDENCIES}/imgui/examples/imgui_impl_opengl3.cpp
    ${DEPENDENCIES}/imgui/examples/imgui_impl_win32.h
    ${DEPENDENCIES}/imgui/examples/imgui_impl_win32.cpp
    ${DEPENDENCIES}/ImGuizmo/*.h
    ${DEPENDENCIES}/ImGuizmo/*.cpp
	) 

ADD_LIBRARY(${SUB_NAME} 
    ${SRC})

TARGET_COMPILE_DEFINITIONS(${SUB_NAME} PUBLIC
	UNICODE=1
	_UNICODE=1
    USE_GL_ES3
    IMGUI_IMPL_OPENGL_ES3
    GL_GLEXT_PROTOTYPES
	)

TARGET_INCLUDE_DIRECTORIES(${SUB_NAME} PUBLIC
    ../glm
    ../imgui
    ../scene
	${CMAKE_INSTALL_PREFIX}/include
	)

