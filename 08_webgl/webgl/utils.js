function getWebGLUniformType(gl, type) {
	const typeMap = {
		[gl.FLOAT]: 'FLOAT',
		[gl.FLOAT_VEC2]: 'FLOAT_VEC2',
		[gl.FLOAT_VEC3]: 'FLOAT_VEC3',
		[gl.FLOAT_VEC4]: 'FLOAT_VEC4',
		[gl.INT]: 'INT',
		[gl.INT_VEC2]: 'INT_VEC2',
		[gl.INT_VEC3]: 'INT_VEC3',
		[gl.INT_VEC4]: 'INT_VEC4',
		[gl.FLOAT_MAT2]: 'FLOAT_MAT2',
		[gl.FLOAT_MAT3]: 'FLOAT_MAT3',
		[gl.FLOAT_MAT4]: 'FLOAT_MAT4',
		[gl.SAMPLER_2D]: 'SAMPLER_2D',
		[gl.SAMPLER_CUBE]: 'SAMPLER_CUBE',
	};

	return typeMap[type] || 'UNKNOWN';
}

function listUniforms(gl, shaderProgram) {
	const numUniforms = gl.getProgramParameter(shaderProgram, gl.ACTIVE_UNIFORMS);
	const uniforms = {};

	for (let i = 0; i < numUniforms; i++) {
		const uniformInfo = gl.getActiveUniform(shaderProgram, i);
		if (!uniformInfo) {
			break;
		}

		// Create a readable type name
		const typeName = getWebGLUniformType(gl, uniformInfo.type);

		// Get the location of the uniform
		const location = gl.getUniformLocation(shaderProgram, uniformInfo.name);

		// Add the uniform information to the object
		uniforms[uniformInfo.name] = {
			type: typeName,
			size: uniformInfo.size,
			location: location
		};
	}

	return uniforms;
}

function loadShader(gl, type, source) {
	const shader = gl.createShader(type);
	gl.shaderSource(shader, source);
	gl.compileShader(shader);

	if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
		alert('An error occurred compiling the shaders: ' + gl.getShaderInfoLog(shader));
		gl.deleteShader(shader);
		return null;
	}

	return shader;
}

function initShaderProgram(gl, vsSource, fsSource) {
	const vertexShader = loadShader(gl, gl.VERTEX_SHADER, vsSource);
	const fragmentShader = loadShader(gl, gl.FRAGMENT_SHADER, fsSource);

	const shaderProgram = gl.createProgram();
	gl.attachShader(shaderProgram, vertexShader);
	gl.attachShader(shaderProgram, fragmentShader);
	gl.linkProgram(shaderProgram);

	// Check if the shader program linked successfully
	if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
		alert('Unable to initialize the shader program: ' + gl.getProgramInfoLog(shaderProgram));
		return null;
	}

	return {
		program: shaderProgram,
		uniformLocations: listUniforms(gl, shaderProgram)
	};
}

function loadTexture(gl, url) {
	const texture = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D, texture);

	// Because images have to be download over the internet
	// they might take a moment until they are ready.
	// Until then put a single pixel in the texture so we can
	// use it immediately. When the image has finished downloading
	// we'll update the texture with the contents of the image.
	const level = 0;
	const internalFormat = gl.RGBA;
	const width = 1;
	const height = 1;
	const border = 0;
	const srcFormat = gl.RGBA;
	const srcType = gl.UNSIGNED_BYTE;
	const pixel = new Uint8Array([0, 0, 255, 255]);  // opaque blue
	gl.texImage2D(gl.TEXTURE_2D, level, internalFormat, width, height, border, srcFormat, srcType, pixel);

	const image = new Image();
	image.onload = function() {
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.texImage2D(gl.TEXTURE_2D, level, internalFormat, gl.RGBA, gl.UNSIGNED_BYTE, image);

		// WebGL1 has different requirements for power of 2 images
		// vs non power of 2 images so check if the image is a
		// power of 2 in both dimensions.
		if (isPowerOf2(image.width) && isPowerOf2(image.height)) {
			// Yes, it's a power of 2. Generate mips.
			gl.generateMipmap(gl.TEXTURE_2D);
		} else {
			// No, it's not a power of 2. Turn off mips and set
			// wrapping to clamp to edge
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
		}
	};
	image.src = url;

	return texture;
}

function isPowerOf2(value) {
	return (value & (value - 1)) == 0;
}

function createNormalMatrix(modelMatrix) {
	const mat3 = glMatrix.mat3;
	const mat4 = glMatrix.mat4;

	// Create a 3x3 normal matrix
	let normalMatrix = mat3.create();

	// Convert modelMatrix (4x4) to a 3x3 matrix
	let modelMatrix3x3 = mat3.create();
	mat3.fromMat4(modelMatrix3x3, modelMatrix);

	// Calculate the inverse of the 3x3 model matrix
	mat3.invert(normalMatrix, modelMatrix3x3);

	// Transpose the inverted matrix
	mat3.transpose(normalMatrix, normalMatrix);

	return normalMatrix;
}

// Function to update model matrix by rotating around the Z-axis
function rotateZ(m, angle) {
	glMatrix.mat4.rotateZ(m, m, angle);
}

// Function to update model matrix by rotating around the Y-axis
function rotateY(m, angle) {
	glMatrix.mat4.rotateY(m, m, angle);
}

// Function to update model matrix by rotating around the X-axis
function rotateX(m, angle) {
	glMatrix.mat4.rotateX(m, m, angle);
}


function renderVAO(gl, vao, count) {
    gl.bindVertexArray(vao);
    gl.drawElements(gl.TRIANGLES, count, gl.UNSIGNED_SHORT, 0);
    gl.bindVertexArray(null);
}

function getTextContent( elementID ) {
    let element = document.getElementById(elementID);
    let node = element.firstChild;
    let str = "";
    while (node) {
        if (node.nodeType == 3) // this is a text node
            str += node.textContent;
        node = node.nextSibling;
    }
    return str;
}
