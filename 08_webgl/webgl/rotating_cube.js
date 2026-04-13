/*============= Creating a canvas =================*/
var canvas = document.getElementById('webglCanvas');
// gl = canvas.getContext('experimental-webgl');

var gl = canvas.getContext('webgl');

if (!gl) {
	alert('WebGL not supported, falling back on experimental-webgl');
	gl = canvas.getContext('experimental-webgl');
}
if (!gl) {
	alert('Your browser does not support WebGL');
}


/*============ Defining and storing the geometry =========*/

var vertices = [
	-1,-1,-1, 1,-1,-1, 1, 1,-1, -1, 1,-1,
	-1,-1, 1, 1,-1, 1, 1, 1, 1, -1, 1, 1,
	-1,-1,-1, -1, 1,-1, -1, 1, 1, -1,-1, 1,
	1,-1,-1, 1, 1,-1, 1, 1, 1, 1,-1, 1,
	-1,-1,-1, -1,-1, 1, 1,-1, 1, 1,-1,-1,
	-1, 1,-1, -1, 1, 1, 1, 1, 1, 1, 1,-1, 
];

var colors = [
	5,3,7, 5,3,7, 5,3,7, 5,3,7,
	1,1,3, 1,1,3, 1,1,3, 1,1,3,
	0,0,1, 0,0,1, 0,0,1, 0,0,1,
	1,0,0, 1,0,0, 1,0,0, 1,0,0,
	1,1,0, 1,1,0, 1,1,0, 1,1,0,
	0,1,0, 0,1,0, 0,1,0, 0,1,0
];

var indices = [
	0,1,2, 0,2,3, 4,5,6, 4,6,7,
	8,9,10, 8,10,11, 12,13,14, 12,14,15,
	16,17,18, 16,18,19, 20,21,22, 20,22,23 
];

// Create and store data into vertex buffer
var vertex_buffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, vertex_buffer);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);

// Create and store data into color buffer
var color_buffer = gl.createBuffer();
gl.bindBuffer(gl.ARRAY_BUFFER, color_buffer);
gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(colors), gl.STATIC_DRAW);

// Create and store data into index buffer
var index_buffer = gl.createBuffer();
gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, index_buffer);
gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, new Uint16Array(indices), gl.STATIC_DRAW);

/*=================== Shaders =========================*/

var vertCode = 'attribute vec3 position;'+
	'uniform mat4 u_modelMat;'+
	'uniform mat4 u_viewMat;'+
	'uniform mat4 u_projMat;'+
	'attribute vec3 color;'+//the color of the point
	'varying vec3 vColor;'+

	'void main(void) { '+//pre-built function
	'gl_Position = u_projMat*u_viewMat*u_modelMat*vec4(position, 1.);'+
	'vColor = color;'+
	'}';

var fragCode = 'precision mediump float;'+
	'varying vec3 vColor;'+
	'void main(void) {'+
	'gl_FragColor = vec4(vColor, 1.);'+
	'}';

var shaderProgram = initShaderProgram(gl, vertCode, fragCode)

/* ====== Associating attributes to vertex shader =====*/
var u_projMat = shaderProgram.uniformLocations.u_projMat.location;
var u_viewMat = shaderProgram.uniformLocations.u_viewMat.location;
var u_modelMat = shaderProgram.uniformLocations.u_modelMat.location;

gl.bindBuffer(gl.ARRAY_BUFFER, vertex_buffer);
var position = gl.getAttribLocation(shaderProgram.program, "position");
gl.vertexAttribPointer(position, 3, gl.FLOAT, false,0,0) ;

// Position
gl.enableVertexAttribArray(position);
gl.bindBuffer(gl.ARRAY_BUFFER, color_buffer);
var color = gl.getAttribLocation(shaderProgram.program, "color");
gl.vertexAttribPointer(color, 3, gl.FLOAT, false,0,0) ;

// Color
gl.enableVertexAttribArray(color);
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
	rotateZ(modelMatrix, dt*0.005);//time
	rotateY(modelMatrix, dt*0.002);
	rotateX(modelMatrix, dt*0.003);
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
	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, index_buffer);
	gl.drawElements(gl.TRIANGLES, indices.length, gl.UNSIGNED_SHORT, 0);

	window.requestAnimationFrame(animate);
}
animate(0);

