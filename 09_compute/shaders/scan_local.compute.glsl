#version 430 core

layout(local_size_x = 256) in; // Define work group size - should match your dispatch and data handling assumptions

layout(std430, binding = 0) buffer InputBuffer {
	int data[];
};

layout(std430, binding = 1) buffer OutputBuffer {
	int aux[];
};

shared int temp[256]; // Shared memory for inter-thread communication within the work group

void main() {
	int local_id = int(gl_LocalInvocationID.x);
	int global_id = int(gl_GlobalInvocationID.x);

	// Load data into shared memory
	temp[local_id] = data[global_id];
	barrier(); // Ensure all data is loaded into shared memory before proceeding

	// Perform the scan operation using shared memory (Blelloch scan)
	// Up-sweep (reduce) phase
	for (int stride = 1; stride < 256; stride *= 2) {
		barrier(); // Wait for all threads to complete their work
		if (local_id % (2 * stride) == (2 * stride - 1)) {
			temp[local_id] += temp[local_id - stride];
		}
	}

	// Down-sweep phase
	if (local_id == 255) { // Set last element to zero before the down-sweep
		temp[255] = 0;
	}
	for (int stride = 128; stride > 0; stride /= 2) {
		barrier(); // Wait for all threads to complete their work
		if (local_id % (2 * stride) == (2 * stride - 1)) {
			int t = temp[local_id - stride];
			temp[local_id - stride] = temp[local_id];
			temp[local_id] += t;
		}
	}

	barrier(); // Make sure all additions are finished

	// Write back the results from shared memory to global memory
	data[global_id] = temp[local_id];
	// Write the sum of this work group to the auxiliary buffer
	if (local_id == 255) {
		aux[gl_WorkGroupID.x] = temp[255];
	}
}
