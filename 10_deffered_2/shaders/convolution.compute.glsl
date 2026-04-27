#version 430 core
layout(local_size_x = 16, local_size_y = 16) in;

layout(rgba32f, binding = 0) uniform readonly image2D inputImage;
layout(rgba32f, binding = 1) uniform writeonly image2D outputImage;

uniform float kernel[9]; // 3x3 convolution kernel
/* const float kernel[9] = float[9]( */
/*     0,  1, 0, */
/*     1, -4, 1, */
/*     0,  1, 0 */
/*     ); */


void main() {
	ivec2 texSize = imageSize(inputImage);
	ivec2 gid = ivec2(gl_GlobalInvocationID.xy);

	if (gid.x >= texSize.x || gid.y >= texSize.y) {
		return; // Skip out-of-bounds work items
	}

	vec4 sum = vec4(0.0);
	int k = 0;

	// Loop over each kernel element
	for (int j = -1; j <= 1; j++) {
		for (int i = -1; i <= 1; i++) {
			ivec2 kernelPos = ivec2(i, j);
			ivec2 imagePos = gid + kernelPos;

			// Handle boundary cases by clamping to the edge of the image
			imagePos = clamp(imagePos, ivec2(0, 0), texSize - ivec2(1, 1));

			vec4 imageColor = imageLoad(inputImage, imagePos);
			sum += imageColor * kernel[k++];
		}
	}

	// Write the result to the output image
	imageStore(outputImage, gid, sum);
}

