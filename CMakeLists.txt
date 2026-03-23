cmake_minimum_required(VERSION 3.10)

# Set your project name
project(GLTutorials)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find OpenGL package
set(OpenGL_GL_PREFERENCE GLVND)
find_package(OpenGL REQUIRED)


if(WIN32)
	# Attempt to find the GLFW root directory
	find_path(GLFW_ROOT_DIR NAMES include/GLFW/glfw3.h
		PATHS
			"${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.4.bin.WIN64"
			"${CMAKE_CURRENT_SOURCE_DIR}/glfw-3.4"
			"${CMAKE_CURRENT_SOURCE_DIR}/../glfw-3.4.bin.WIN64"
			"${CMAKE_CURRENT_SOURCE_DIR}/../glfw-3.4"
		NO_DEFAULT_PATH) # Avoid searching in default paths

	if(GLFW_ROOT_DIR)
		# Determine the appropriate library directory based on the Visual Studio version
		if(MSVC_VERSION GREATER_EQUAL 1930 AND MSVC_VERSION LESS 1950)
			set(GLFW_LIB_DIR "${GLFW_ROOT_DIR}/lib-vc2022")
		elseif(MSVC_VERSION GREATER_EQUAL 1920 AND MSVC_VERSION LESS 1930)
			set(GLFW_LIB_DIR "${GLFW_ROOT_DIR}/lib-vc2019")
		elseif(MSVC_VERSION GREATER_EQUAL 1910 AND MSVC_VERSION LESS 1920)
			set(GLFW_LIB_DIR "${GLFW_ROOT_DIR}/lib-vc2017")
			# Add more conditions for other Visual Studio versions as needed
		else()
			message(FATAL_ERROR "Unsupported Visual Studio version for GLFW.")
		endif()

		add_library(glfw INTERFACE)

		# Set the include directory for the glfw target
		target_include_directories(glfw INTERFACE "${GLFW_ROOT_DIR}/include")

		# Assuming the GLFW library filename; adjust as necessary
		find_library(GLFW_LIBRARY NAMES glfw3 PATHS "${GLFW_LIB_DIR}" NO_DEFAULT_PATH)

		if(NOT GLFW_LIBRARY)
			message(FATAL_ERROR "GLFW library not found.")
		endif()

		# Link the library with the glfw target
		target_link_libraries(glfw INTERFACE "${GLFW_LIBRARY}")

		# Specify the include directory
		set(GLFW_INCLUDE_DIR "${GLFW_ROOT_DIR}/include")
	else()
		message(FATAL_ERROR "GLFW root directory not found.")
	endif()

	find_path(GLM_ROOT_DIR NAMES glm/glm.hpp
		PATHS
			"${CMAKE_CURRENT_SOURCE_DIR}/glm-1.0.1"
			"${CMAKE_CURRENT_SOURCE_DIR}/../glm-1.0.1"
		NO_DEFAULT_PATH)

	if (NOT GLM_ROOT_DIR)
		message(FATAL_ERROR "GLM root directory not found.")
	endif()

	add_library(glm INTERFACE)
	target_include_directories(glm INTERFACE "${GLM_ROOT_DIR}")
	target_compile_definitions(glm INTERFACE GLM_ENABLE_EXPERIMENTAL GLM_FORCE_SWIZZLE)
	add_library(glm::glm ALIAS glm)
else()
	find_package(glfw3 3.3 REQUIRED)
	find_package(glm REQUIRED)
	add_compile_definitions(GLM_FORCE_SWIZZLE GLM_ENABLE_EXPERIMENTAL)
endif()


add_library(utils
	utils/ogl_material_factory.cpp
	utils/ogl_geometry_factory.cpp
	utils/ogl_geometry_construction.cpp
	utils/obj_file_loading.cpp
	)
target_link_libraries(utils glm::glm glfw OpenGL::GL)
target_include_directories(utils PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR}/glad/include
	${CMAKE_CURRENT_SOURCE_DIR}
)


# Add subdirectories here
add_subdirectory(01_triangle)
add_subdirectory(02_simple3d)
add_subdirectory(03_textures)
add_subdirectory(04_deffered)
add_subdirectory(05_shaders)
