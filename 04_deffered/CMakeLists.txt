# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

project(04_deffered)

add_executable(04_deffered 
	main.cpp
	renderer.hpp
	scene_definition.hpp
)
target_sources(04_deffered PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c
)
target_link_libraries(04_deffered utils glm::glm glfw OpenGL::GL)
target_include_directories(04_deffered PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/../utils
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}
)
