#version 430 core
layout(local_size_x = 16, local_size_y = 16) in;


layout(rgba32f, binding = 0) uniform readonly image2D inputImage;
layout(rgba32f, binding = 1) uniform writeonly image2D outputImage;

/*
const float kernel[25] = float[](
	1.0/273,  4.0/273,  7.0/273,  4.0/273, 1.0/273,
	4.0/273, 16.0/273, 26.0/273, 16.0/273, 4.0/273,
	7.0/273, 26.0/273, 41.0/273, 26.0/273, 7.0/273,
	4.0/273, 16.0/273, 26.0/273, 16.0/273, 4.0/273,
	1.0/273,  4.0/273,  7.0/273,  4.0/273, 1.0/273
);
 */


void main() {
	ivec2 texSize = imageSize(inputImage);
	ivec2 gid = ivec2(gl_GlobalInvocationID.xy);

	if (gid.x >= texSize.x || gid.y >= texSize.y) {
		return; // Skip out-of-bounds work items
	}

	// TODO - convolution
	vec4 color = imageLoad(inputImage, gid);

	// Write the result to the output image
	imageStore(outputImage, gid, vec4(vec3(1.0) - color.xyz, 1.0));
}
