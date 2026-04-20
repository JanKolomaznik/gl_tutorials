#version 430 core

layout(local_size_x = 1) in;

layout(std430, binding = 1) buffer AuxBuffer {
    int aux[]; // This buffer holds the last element of each work group from the first shader
};

layout(std430, binding = 0) buffer OutputBuffer {
    int data[]; // Buffer that holds the scanned array
};

shared int temp[256]; // Shared memory for inter-thread communication within the work group

void main() {
    int index = int(gl_GlobalInvocationID.x);
    
    // Perform a prefix sum on the auxiliary buffer
    if (index == 0) {
        for (int i = 1; i < aux.length(); i++) {
            aux[i] += aux[i - 1];
        }
    }

    barrier(); // Synchronize all operations on the aux buffer

    // Adjust elements in the main buffer
    if (index > 0 ) {
        int addValue = aux[index - 1];
        int start = index * 256; // Assuming the local_size_x of the first shader is 256
        int end = start + 256;
        for (int i = start; i < end; i++) {
            // data[i] += addValue;
            data[i] = addValue;
        }
    }
}

// #version 430 core
//
// layout(local_size_x = 1) in;
//
// layout(std430, binding = 0) buffer InputBuffer {
// 	int data[];
// };
//
// layout(std430, binding = 1) buffer AuxBuffer {
// 	int aux[];
// };
//
// void main() {
// 	// Each thread handles one work group's data from the first pass
// 	int group_id = int(gl_GlobalInvocationID.x);
//
// 	// Calculate prefix sum of aux buffer
// 	for (int i = 1; i <= group_id; i++) {
// 		aux[group_id] += aux[i - 1];
// 	}
//
// 	barrier();
//
// 	// Add aux value to each element in the corresponding segment of the data array
// 	int start_index = group_id * 256; // 256 should match local_size_x in the first shader
// 	int end_index = start_index + 256;
// 	for (int i = start_index; i < end_index; i++) {
// 		data[i] += aux[group_id];
// 	}
// }
