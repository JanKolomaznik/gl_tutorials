/*============= Creating a canvas =================*/
var canvas = document.getElementById('webglCanvas');
var gl = canvas.getContext('webgl2');

/*============ Defining and storing the geometry and texture =========*/

var cubeVAO = createCubeVAO(gl);

var texture = loadTexture(gl, 'Pebbles_003_COLOR.jpg');


/*=================== Shaders =========================*/

/*var vertCode = `#version 300 es
uniform mat4 u_modelMat;
uniform mat4 u_viewMat;
uniform mat4 u_projMat;
uniform mat3 u_normalMat;

in vec3 in_vert;
in vec3 in_normal;
in vec2 in_texCoord;

out vec3 f_normal;
out vec3 f_position;
out vec2 f_texCoord;

void main(void)
{
	gl_Position = u_projMat * u_viewMat * u_modelMat * vec4(in_vert, 1);
	f_normal = normalize(u_normalMat * in_normal);
	f_position = vec3(u_modelMat * vec4(in_vert, 1.0));
	f_texCoord = in_texCoord;
}
`;*/

var vertCode = getTextContent("vshader")
var fragCode = getTextContent("fshader")

// var fragCode = `#version 300 es
// precision mediump float;
// uniform sampler2D u_textureSampler;
//
// out vec4 out_fragColor;
//
// in vec3 f_normal;
// in vec2 f_texCoord;
//
// void main() {
// 	out_fragColor = texture(u_textureSampler, f_texCoord);
// 	// out_fragColor = vec4(f_texCoord, 0, 1);
// }
//
// `;
var shaderProgram = initShaderProgram(gl, vertCode, fragCode)

/* ====== Associating attributes to vertex shader =====*/
var u_projMat = shaderProgram.uniformLocations.u_projMat.location;
var u_viewMat = shaderProgram.uniformLocations.u_viewMat.location;
var u_modelMat = shaderProgram.uniformLocations.u_modelMat.location;
var u_normalMat = shaderProgram.uniformLocations.u_normalMat.location;
var u_textureSampler = shaderProgram.uniformLocations.u_textureSampler.location;

gl.useProgram(shaderProgram.program);

/*==================== MATRIX =====================*/

var projectionMatrix = glMatrix.mat4.create();
glMatrix.mat4.perspective(projectionMatrix, 
                     Math.PI / 4,       // field of view in radians
                     canvas.width / canvas.height, // aspect ratio
                     0.1,               // near clip
                     100.0);            // far clip

// Model Matrix
var modelMatrix = glMatrix.mat4.create(); // Starts with an identity matrix

var viewMatrix = glMatrix.mat4.create();
glMatrix.mat4.lookAt(viewMatrix, 
	[0, 0, -10], // Camera is at (0,0,-10), in World Space
	[0, 0, 0],   // and looks at the origin
	[0, 1, 0]);  // Head is up (set to 0,-1,0 to look upside-down)


/*================= Drawing ===========================*/
var time_old = 0;

var animate = function(time) {

	var dt = time-time_old;
	rotateZ(modelMatrix, dt*0.001);//time
	rotateY(modelMatrix, dt*0.0005);
	rotateX(modelMatrix, dt*0.0003);
	time_old = time;

	gl.enable(gl.DEPTH_TEST);
	gl.depthFunc(gl.LEQUAL);
	gl.clearColor(0.2, 0.5, 0.6, 0.9);
	gl.clearDepth(1.0);

	gl.viewport(0.0, 0.0, canvas.width, canvas.height);
	gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	gl.uniformMatrix4fv(u_projMat, false, projectionMatrix);
	gl.uniformMatrix4fv(u_viewMat, false, viewMatrix);
	gl.uniformMatrix4fv(u_modelMat, false, modelMatrix);
	gl.uniformMatrix3fv(u_normalMat, false, createNormalMatrix(modelMatrix));

	gl.bindTexture(gl.TEXTURE_2D, texture);
	gl.uniform1i(u_textureSampler, 0);

	renderVAO(gl, cubeVAO.vao, cubeVAO.count)
	// gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, index_buffer);
	// gl.drawElements(gl.TRIANGLES, indices.length, gl.UNSIGNED_SHORT, 0);

	window.requestAnimationFrame(animate);
}
animate(0);

