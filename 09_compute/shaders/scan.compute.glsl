#version 430 core

// Naive scan implementation.
// It uses only global memory and work group synchronization,
// Must be executed only in one workgroup - no global synchronization

layout(local_size_x = 256) in;

layout(std430, binding = 0) buffer Input {
    int data[];
};

void main() {
    uint idx = gl_GlobalInvocationID.x;
    barrier();
    for (uint stride = 1; stride < gl_NumWorkGroups.x * gl_WorkGroupSize.x; stride *= 2) {
        int temp = 0;
        if (idx >= stride) {
            temp = data[idx - stride];
        }
        barrier();
        if (idx >= stride) {
            data[idx] += temp;
        }
        barrier();
    }
}
