# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

project(09_compute)

add_executable(multiply 
	main.cpp
)
target_sources(multiply PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c
)
target_link_libraries(multiply utils glm::glm glfw OpenGL::GL)
target_include_directories(multiply PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/../utils
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(scan
	scan.cpp
)
target_sources(scan PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c
)
target_link_libraries(scan utils glm::glm glfw OpenGL::GL)
target_include_directories(scan PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/../utils
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(convolution
	convolution.cpp
)
target_sources(convolution PRIVATE 
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/src/glad.c
)
target_link_libraries(convolution utils glm::glm glfw OpenGL::GL)
target_include_directories(convolution PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/../glad/include
	${CMAKE_CURRENT_SOURCE_DIR}/../utils
	${CMAKE_CURRENT_SOURCE_DIR}
)

