export function createGLContext(canvas) {
	const gl = canvas.getContext("webgl2");
	if (!gl) throw new Error("WebGL2 not supported");
	canvas.width = window.innerWidth;
	canvas.height = window.innerHeight;
	return gl;
}

export function compileShader(gl, source, type) {
	const shader = gl.createShader(type);
	gl.shaderSource(shader, source);
	gl.compileShader(shader);
	if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
		throw new Error(gl.getShaderInfoLog(shader));
	}
	return shader;
}

export function loadShaderProgram(gl, vsSource, fsSource) {
	const vertexShader = compileShader(gl, vsSource, gl.VERTEX_SHADER);
	const fragmentShader = compileShader(gl, fsSource, gl.FRAGMENT_SHADER);
	const program = gl.createProgram();
	gl.attachShader(program, vertexShader);
	gl.attachShader(program, fragmentShader);
	gl.linkProgram(program);
	if (!gl.getProgramParameter(program, gl.LINK_STATUS)) {
		throw new Error(gl.getProgramInfoLog(program));
	}
	return program;
}


export function loadTexture(gl, url) {
	const texture = gl.createTexture();
	gl.bindTexture(gl.TEXTURE_2D, texture);

	// Placeholder pixel while image loads
	const level = 0;
	const internalFormat = gl.RGBA;
	const width = 1;
	const height = 1;
	const border = 0;
	const srcFormat = gl.RGBA;
	const srcType = gl.UNSIGNED_BYTE;
	const placeholderPixel = new Uint8Array([255, 0, 255, 255]); // magenta debug color
	gl.texImage2D(gl.TEXTURE_2D, level, internalFormat,
		width, height, border, srcFormat, srcType,
		placeholderPixel);

	// Load actual image
	const image = new Image();
	image.onload = () => {
		gl.bindTexture(gl.TEXTURE_2D, texture);
		gl.texImage2D(gl.TEXTURE_2D, level, internalFormat,
			srcFormat, srcType, image);

		if (isPowerOf2(image.width) && isPowerOf2(image.height)) {
			gl.generateMipmap(gl.TEXTURE_2D);
		} else {
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE);
			gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR);
		}
	};
	image.src = url;

	return texture;
}

function isPowerOf2(value) {
	return (value & (value - 1)) === 0;
}
