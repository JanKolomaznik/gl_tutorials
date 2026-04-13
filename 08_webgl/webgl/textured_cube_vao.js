

function createCubeVAO(gl) {
	const vertices = new Float32Array([
		// Positions         // Normals          // Texture Coordinates
		// Front face
		-1.0, -1.0,  1.0,    0.0,  0.0,  1.0,    0.0, 0.0,
		1.0, -1.0,  1.0,    0.0,  0.0,  1.0,    1.0, 0.0,
		1.0,  1.0,  1.0,    0.0,  0.0,  1.0,    1.0, 1.0,
		-1.0,  1.0,  1.0,    0.0,  0.0,  1.0,    0.0, 1.0,
		// Back face
		-1.0, -1.0, -1.0,    0.0,  0.0, -1.0,    1.0, 0.0,
		1.0, -1.0, -1.0,    0.0,  0.0, -1.0,    0.0, 0.0,
		1.0,  1.0, -1.0,    0.0,  0.0, -1.0,    0.0, 1.0,
		-1.0,  1.0, -1.0,    0.0,  0.0, -1.0,    1.0, 1.0,
		// Top face
		-1.0,  1.0, -1.0,    0.0,  1.0,  0.0,    0.0, 1.0,
		-1.0,  1.0,  1.0,    0.0,  1.0,  0.0,    0.0, 0.0,
		1.0,  1.0,  1.0,    0.0,  1.0,  0.0,    1.0, 0.0,
		1.0,  1.0, -1.0,    0.0,  1.0,  0.0,    1.0, 1.0,
		// Bottom face
		-1.0, -1.0, -1.0,    0.0, -1.0,  0.0,    1.0, 1.0,
		-1.0, -1.0,  1.0,    0.0, -1.0,  0.0,    0.0, 1.0,
		1.0, -1.0,  1.0,    0.0, -1.0,  0.0,    0.0, 0.0,
		1.0, -1.0, -1.0,    0.0, -1.0,  0.0,    1.0, 0.0,
		// Right face
		1.0, -1.0, -1.0,    1.0,  0.0,  0.0,    1.0, 1.0,
		1.0, -1.0,  1.0,    1.0,  0.0,  0.0,    1.0, 0.0,
		1.0,  1.0,  1.0,    1.0,  0.0,  0.0,    0.0, 0.0,
		1.0,  1.0, -1.0,    1.0,  0.0,  0.0,    0.0, 1.0,
		// Left face
		-1.0, -1.0, -1.0,   -1.0,  0.0,  0.0,    0.0, 1.0,
		-1.0, -1.0,  1.0,   -1.0,  0.0,  0.0,    1.0, 1.0,
		-1.0,  1.0,  1.0,   -1.0,  0.0,  0.0,    1.0, 0.0,
		-1.0,  1.0, -1.0,   -1.0,  0.0,  0.0,    0.0, 0.0
	]);

	const indices = new Uint16Array([
		0, 1, 2, 0, 2, 3,    // front
		4, 5, 6, 4, 6, 7,    // back
		8, 9, 10, 8, 10, 11, // top
		12, 13, 14, 12, 14, 15, // bottom
		16, 17, 18, 16, 18, 19, // right
		20, 21, 22, 20, 22, 23  // left
	]);

	const vertexBuffer = gl.createBuffer();
	const indexBuffer = gl.createBuffer();
	const vao = gl.createVertexArray();

	gl.bindVertexArray(vao);

	gl.bindBuffer(gl.ARRAY_BUFFER, vertexBuffer);
	gl.bufferData(gl.ARRAY_BUFFER, vertices, gl.STATIC_DRAW);

	gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, indexBuffer);
	gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, indices, gl.STATIC_DRAW);

	const positionSize = 3;
	const normalSize = 3;
	const texCoordSize = 2;
	const vertexSize = (positionSize + normalSize + texCoordSize) * Float32Array.BYTES_PER_ELEMENT;
	const normalOffset = positionSize * Float32Array.BYTES_PER_ELEMENT;
	const texCoordOffset = (positionSize + normalSize) * Float32Array.BYTES_PER_ELEMENT;

	// Position attribute
	gl.vertexAttribPointer(0, positionSize, gl.FLOAT, false, vertexSize, 0);
	gl.enableVertexAttribArray(0);

	// Normal attribute
	gl.vertexAttribPointer(1, normalSize, gl.FLOAT, false, vertexSize, normalOffset);
	gl.enableVertexAttribArray(1);

	// Texture coordinate attribute
	gl.vertexAttribPointer(2, texCoordSize, gl.FLOAT, false, vertexSize, texCoordOffset);
	gl.enableVertexAttribArray(2);

	gl.bindVertexArray(null); // Unbind the VAO

	return {
		vao: vao,
		count: indices.length
	};
}
