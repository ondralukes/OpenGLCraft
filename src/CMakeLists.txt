cmake_minimum_required(VERSION 3.10)

# set the project name
project(OpenGLCraft)

find_package(OpenGL REQUIRED)

add_subdirectory (libs)

set(GCC_COVERAGE_COMPILE_FLAGS " -lstdc++fs")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

include_directories(
	libs/glfw-3.2.1/include/
  libs/glew-2.1.0/include/
	libs/glm/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	glfw
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
add_executable(main
	main.cpp
  chunk.cpp
  structs.cpp
	loadShaders.cpp
	loadObj.cpp
	blockArray.cpp
	block.cpp
	ray.cpp
	text.cpp
	saveManager.cpp
	worldGenerator.cpp
	loadTexture.cpp
	resourceManager.cpp
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
