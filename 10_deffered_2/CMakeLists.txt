# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

project(10_deffered_2)

add_executable(10_deffered_2 
	main.cpp
	renderer.hpp
	scene_definition.hpp
)
target_sources(10_deffered_2 PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c
)
target_link_libraries(10_deffered_2 utils glm::glm glfw OpenGL::GL)
target_include_directories(10_deffered_2 PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/../utils
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}
)
