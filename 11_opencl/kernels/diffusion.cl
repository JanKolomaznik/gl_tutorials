
__kernel void uchar_to_float(__global const uchar* input, __global float* output, int size) {
	int index = get_global_id(0);
	if (index < size) {
		output[index] = input[index] / 255.0f;
	}
}

__kernel void float_to_uchar(__global const float* input, __global uchar* output, int size) {
	int index = get_global_id(0);
	if (index < size) {
		output[index] = (uchar)(input[index] * 255.0f);
	}
}

__kernel void diffusion_step(__global float* current, __global float* next, int width, int height, float delta) {
	int x = get_global_id(0);
	int y = get_global_id(1);
	int idx = y * width + x;

	float center = current[idx];
	float diffusion = 0.0f;
	int count = 0;

	// Consider the 4 neighboring pixels (left, right, top, bottom)
	if (x > 0) {
		diffusion += current[idx - 1];
		count++;
	}
	if (x < width - 1) {
		diffusion += current[idx + 1];
		count++;
	}
	if (y > 0) {
		diffusion += current[idx - width];
		count++;
	}
	if (y < height - 1) {
		diffusion += current[idx + width];
		count++;
	}

	// Apply Laplacian diffusion
	float laplacian = diffusion / count - center;
	next[idx] = center + delta * laplacian;
}
