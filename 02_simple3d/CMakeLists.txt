# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

project(02_simple3d)

add_executable(02_simple3d 
	main.cpp
	../utils/error_handling.hpp
	../utils/ogl_resource.hpp
	../utils/shader.hpp
	../utils/window.hpp
)
target_sources(02_simple3d PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c
)
target_link_libraries(02_simple3d utils glm::glm glfw OpenGL::GL)
target_include_directories(02_simple3d PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/../utils
	${CMAKE_CURRENT_SOURCE_DIR}
)
