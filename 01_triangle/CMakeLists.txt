# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

project(01_triangle)

add_executable(01_triangle_vanilla vanilla.cpp)
target_sources(01_triangle_vanilla PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c)
target_link_libraries(01_triangle_vanilla glfw OpenGL::GL)
target_include_directories(01_triangle_vanilla PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/../utils
)

add_executable(01_triangle 
	main.cpp
	coloredTriangle.hpp
	fractal.hpp
	simpleTriangle.hpp
	../utils/error_handling.hpp
	../utils/ogl_resource.hpp
	../utils/shader.hpp
	../utils/window.hpp
)
target_sources(01_triangle PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c)
target_link_libraries(01_triangle glm::glm glfw OpenGL::GL)
target_include_directories(01_triangle PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/../utils
)
