# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

project(07_3d_textures)

add_executable(07_3d_textures
	main.cpp
	../utils/error_handling.hpp
	../utils/ogl_resource.hpp
	../utils/shader.hpp
	../utils/window.hpp
)
target_sources(07_3d_textures PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c
)
target_link_libraries(07_3d_textures utils glm::glm glfw OpenGL::GL)
target_include_directories(07_3d_textures PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/../utils
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}
)
