cmake_minimum_required(VERSION 3.10)

# set the project name
project(OpenGLCraft)

find_package(OpenGL REQUIRED)

execute_process(COMMAND git describe --tags --abbrev=8 --long --dirty --broken OUTPUT_VARIABLE OGLC_VERSION)
string(REGEX REPLACE "\n$" "" OGLC_VERSION "${OGLC_VERSION}")

add_compile_definitions(OGLC_VERSION="${OGLC_VERSION}")


add_subdirectory (libs)

set(GCC_COVERAGE_COMPILE_FLAGS " -lstdc++fs")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

include_directories(
	libs/glfw-3.2.1/include/
  libs/glew-2.1.0/include/
	libs/glm/
	libs/zlib
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
	zlib
	GLEW_210
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

# add the executable

file(GLOB ALL_BLOCKS
  "${CMAKE_SOURCE_DIR}/blocks/*"
)

add_executable(main
	main.cpp
  chunk.cpp
  structs.cpp
	loadShaders.cpp
	loadObj.cpp
	blockArray.cpp
	blockUtils.cpp
	ray.cpp
	gui/gui.cpp
	gui/guiImage.cpp
	text.cpp
	inventory.cpp
	droppedBlock.cpp
	saveManager.cpp
	worldGenerator.cpp
	loadTexture.cpp
	resourceManager.cpp
	${ALL_BLOCKS}
)

target_link_libraries(main
	${ALL_LIBS}
)

file(GLOB ALL_SHADERS
  "${CMAKE_SOURCE_DIR}/shaders/*"
)
FILE(COPY ${ALL_SHADERS} DESTINATION ${CMAKE_BINARY_DIR}/shaders/)

file(GLOB ALL_TEXTURES
  "${CMAKE_SOURCE_DIR}/textures/*"
)
FILE(COPY ${ALL_TEXTURES} DESTINATION ${CMAKE_BINARY_DIR}/textures/)
