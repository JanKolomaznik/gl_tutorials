#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <stb/stb_image.h>
#include <stb/stb_image_write.h>
#include <filesystem>

// Function to save a buffer to a PNG file with a zero-padded index
inline void save_png(const std::filesystem::path& directory, const std::vector<cl_uchar>& buffer, int width, int height, int channels, int iteration) {
	if (!std::filesystem::exists(directory)) {
		std::filesystem::create_directories(directory);
	}
	// std::vector<cl_uchar> tmp(buffer.size());
	// std::transform(buffer.begin(), buffer.end(), tmp.begin(), [](const auto &val) { return val * 255; });
	std::ostringstream filename;
	filename << "output_" << std::setw(3) << std::setfill('0') << iteration << ".png";
	stbi_write_png((directory / filename.str()).c_str(), width, height, channels, buffer.data(), width);
}

// Function to load a grayscale image from a file
inline std::vector<cl_uchar> load_grayscale_image(const std::filesystem::path& filepath, int& width, int& height) {
	int channels;
	stbi_uc* data = stbi_load(filepath.string().c_str(), &width, &height, &channels, STBI_grey);
	if (!data) {
		throw std::runtime_error("Failed to load image: " + filepath.string());
	}
	if (channels != 1) {
		stbi_image_free(data);
		throw std::runtime_error("Not a grayscale image: " + filepath.string());
	}
	std::vector<cl_uchar> buffer(data, data + width * height);
	stbi_image_free(data);
	return buffer;
}


// Function to load an OpenCL kernel source code from a text file
inline std::string load_kernel(const std::filesystem::path& filepath) {
	std::ifstream file(filepath);
	if (!file.is_open()) {
		throw std::runtime_error("Failed to open kernel file: " + filepath.string());
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return buffer.str();
}

inline void printAvailablePlatformsAndDevices(std::vector<cl::Platform> &platforms) {
	for (size_t i = 0; i < platforms.size(); ++i) {
		std::string platformName = platforms[i].getInfo<CL_PLATFORM_NAME>();
		std::string platformVendor = platforms[i].getInfo<CL_PLATFORM_VENDOR>();
		std::cout << "Platform " << i << ": " << platformName
			<< " (Vendor: " << platformVendor << ")" << std::endl;

		std::vector<cl::Device> devices;
		platforms[i].getDevices(CL_DEVICE_TYPE_ALL, &devices);

		if (devices.empty()) {
			std::cout << "  No devices found on this platform." << std::endl;
			continue;
		}

		for (size_t j = 0; j < devices.size(); ++j) {
			std::string deviceName = devices[j].getInfo<CL_DEVICE_NAME>();
			cl_device_type deviceType = devices[j].getInfo<CL_DEVICE_TYPE>();
			std::string typeStr = (deviceType == CL_DEVICE_TYPE_CPU) ? "CPU" :
				(deviceType == CL_DEVICE_TYPE_GPU) ? "GPU" :
				(deviceType == CL_DEVICE_TYPE_ACCELERATOR) ? "Accelerator" :
				"Other";

			std::cout << "  Device " << j << ": " << deviceName
				<< " [Type: " << typeStr << "]" << std::endl;
		}
	}
}
