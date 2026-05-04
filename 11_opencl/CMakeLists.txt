
# Set the minimum CMake version
cmake_minimum_required(VERSION 3.10)

project(11_opencl)

find_package(OpenCL REQUIRED)

add_executable(11_conway
	conway.cpp
)
target_link_libraries(11_conway OpenCL::OpenCL)
target_include_directories(11_conway PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(11_diffusion
	diffusion.cpp
)
target_link_libraries(11_diffusion OpenCL::OpenCL)
target_include_directories(11_diffusion PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/..
	${CMAKE_CURRENT_SOURCE_DIR}
)
