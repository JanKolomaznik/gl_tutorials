#include <CL/cl.hpp>
//#include <CL/opencl.hpp>
#include <vector>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <random>

#include "cl_utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

const int width = 256;
const int height = 256;
const int iterations = 100;

int main() {
	try {
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		if (platforms.empty()) {
			std::cout << "No OpenCL platforms found." << std::endl;
			return 1;
		}
		printAvailablePlatformsAndDevices(platforms);

		// Initialize OpenCL context, device, and command queue
		cl::Platform platform = cl::Platform::getDefault();
		cl::Device device = cl::Device::getDefault();
		cl::Context context({ device });

		std::filesystem::path kernelPath = "kernels/conway.cl";
		std::string kernelSource = load_kernel(kernelPath);

		cl::Program program(context, kernelSource);
		program.build({ device });
		cl::CommandQueue queue(context, device);

		// Create random initial buffer
		std::vector<cl_uchar> buffer(width * height);
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dis(0, 1);
		for (auto& cell : buffer) {
			cell = dis(gen);
		}

		// Create OpenCL buffers
		cl::Buffer currentBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, buffer.size() * sizeof(cl_uchar), buffer.data());
		cl::Buffer nextBuffer(context, CL_MEM_READ_WRITE, buffer.size() * sizeof(cl_uchar));
		cl::Buffer outputBuffer(context, CL_MEM_WRITE_ONLY, buffer.size() * sizeof(cl_uchar));


		// Set kernel arguments
		cl::Kernel kernel(program, "game_of_life");
		kernel.setArg(0, currentBuffer);
		kernel.setArg(1, nextBuffer);
		kernel.setArg(2, width);
		kernel.setArg(3, height);

		cl::Kernel scaleKernel(program, "scale_to_255");
		scaleKernel.setArg(0, nextBuffer);
		scaleKernel.setArg(1, outputBuffer);
		scaleKernel.setArg(2, static_cast<int>(buffer.size()));

		// Get the initial state
		queue.enqueueWriteBuffer(nextBuffer, CL_TRUE, 0, buffer.size() * sizeof(cl_uchar), buffer.data());
		queue.enqueueNDRangeKernel(scaleKernel, cl::NullRange, cl::NDRange(buffer.size()), cl::NullRange);
		queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, buffer.size() * sizeof(cl_uchar), buffer.data());
		save_png("conway_out", buffer, width, height, 1, 0);

		for (int i = 0; i < iterations; ++i) {
			std::cout << "Creating generation " << i << " ...\n";
			// Execute kernel
			queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(width, height), cl::NullRange);
			// queue.finish();

			// Scale buffer values for black and white output
			queue.enqueueNDRangeKernel(scaleKernel, cl::NullRange, cl::NDRange(buffer.size()), cl::NullRange);
			// queue.finish();

			// Read result
			queue.enqueueReadBuffer(outputBuffer, CL_TRUE, 0, buffer.size() * sizeof(cl_uchar), buffer.data());

			// Save to PNG
			save_png("conway_out", buffer, width, height, 1, i+1);

			// Swap buffers
			std::swap(currentBuffer, nextBuffer);
			kernel.setArg(0, currentBuffer);
			kernel.setArg(1, nextBuffer);
		}
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}
