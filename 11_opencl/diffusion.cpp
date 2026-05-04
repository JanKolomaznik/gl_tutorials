#include <CL/cl.hpp>
//#include <CL/opencl.hpp>
#include <vector>
#include <iostream>
#include <random>

#include "cl_utils.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

const int iterations = 500;
const float delta = 0.2f;  // Diffusion strength

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

		// Load kernel source from file
		std::filesystem::path kernelPath = "kernels/diffusion.cl";
		std::string kernelSource = load_kernel(kernelPath);

		cl::Program program(context, kernelSource);
		program.build({ device });
		cl::CommandQueue queue(context, device);

		// Load initial buffer by loading a grayscale image
		std::vector<cl_uchar> buffer;
		int width, height;
		std::filesystem::path imagePath = "images/input.png";
		buffer = load_grayscale_image(imagePath, width, height);


		// Create OpenCL buffers
		cl::Buffer ucharBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, buffer.size() * sizeof(cl_uchar), buffer.data());
		cl::Buffer floatBufferCurrent(context, CL_MEM_READ_WRITE, buffer.size() * sizeof(cl_float));
		cl::Buffer floatBufferNext(context, CL_MEM_READ_WRITE, buffer.size() * sizeof(cl_float));


		cl::Kernel ucharToFloatKernel(program, "uchar_to_float");
		ucharToFloatKernel.setArg(0, ucharBuffer);
		ucharToFloatKernel.setArg(1, floatBufferCurrent);
		ucharToFloatKernel.setArg(2, static_cast<int>(buffer.size()));

		cl::Kernel floatToUcharKernel(program, "float_to_uchar");
		floatToUcharKernel.setArg(0, floatBufferCurrent);
		floatToUcharKernel.setArg(1, ucharBuffer);
		floatToUcharKernel.setArg(2, static_cast<int>(buffer.size()));

		// Create diffusion kernel
		cl::Kernel diffusionKernel(program, "diffusion_step");
		diffusionKernel.setArg(0, floatBufferCurrent);
		diffusionKernel.setArg(1, floatBufferNext);
		diffusionKernel.setArg(2, width);
		diffusionKernel.setArg(3, height);
		diffusionKernel.setArg(4, delta);

		// Convert uchar to float
		queue.enqueueNDRangeKernel(ucharToFloatKernel, cl::NullRange, cl::NDRange(buffer.size()), cl::NullRange);


		for (int i = 0; i < iterations; ++i) {
			std::cout << "Running iteration " << i << " ...\n";
			// Execute diffusion kernel
			queue.enqueueNDRangeKernel(diffusionKernel, cl::NullRange, cl::NDRange(width, height), cl::NullRange);

			// Swap buffers
			std::swap(floatBufferCurrent, floatBufferNext);
			diffusionKernel.setArg(0, floatBufferCurrent);
			diffusionKernel.setArg(1, floatBufferNext);

			if (i % 50 == 0) {
				// Convert float to uchar
				queue.enqueueNDRangeKernel(floatToUcharKernel, cl::NullRange, cl::NDRange(buffer.size()), cl::NullRange);

				// Read result
				queue.enqueueReadBuffer(ucharBuffer, CL_TRUE, 0, buffer.size() * sizeof(cl_uchar), buffer.data());

				// Save to PNG
				save_png("diffusion_out", buffer, width, height, 1, i);
			}
		}
	} catch (std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return 0;
}
