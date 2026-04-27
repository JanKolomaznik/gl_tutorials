#version 430 core
layout(local_size_x = 16, local_size_y = 16) in;


layout(rgba32f, binding = 0) uniform writeonly image2D outputImage;
layout(rgba32f, binding = 1) uniform readonly image2D u_diffuse;
layout(rgba32f, binding = 2) uniform readonly image2D u_normal;
layout(rgba32f, binding = 3) uniform readonly image2D u_position;
layout(rgba32f, binding = 4) uniform readonly image2D u_shadows;
/* layout(binding = 4) uniform sampler2D u_shadowMap; */

layout(location = 15) uniform vec3 u_lightPos;
layout(location = 20) uniform mat4 u_lightMat;
layout(location = 40) uniform mat4 u_lightProjMat;





void main() {
	ivec2 texSize = imageSize(u_diffuse);
	ivec2 gid = ivec2(gl_GlobalInvocationID.xy);

	if (gid.x >= texSize.x || gid.y >= texSize.y) {
		return; // Skip out-of-bounds work items
	}

	vec4 color = imageLoad(u_diffuse, gid);
	vec3 normal = imageLoad(u_normal, gid).xyz;
	vec3 position = imageLoad(u_position, gid).xyz;
	float shadow = imageLoad(u_shadows, gid).x;

	color = vec4(color.rgb * min(1.0, shadow +0.2), color.a);

	// Write the result to the output image
	imageStore(outputImage, gid, color);
}
