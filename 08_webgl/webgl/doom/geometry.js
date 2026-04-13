export function createEnemyGeometry(gl, program) {
  const vao = gl.createVertexArray();
  gl.bindVertexArray(vao);

  const positions = new Float32Array([
    // positions      // normals       // texCoords
    -0.5, 0, 0,        0, 0, 1,         0, 1,
     0.5, 0, 0,        0, 0, 1,         1, 1,
     0.5, 1.5, 0,      0, 0, 1,         1, 0,
    -0.5, 1.5, 0,      0, 0, 1,         0, 0,
  ]);

  const indices = new Uint16Array([0, 1, 2, 0, 2, 3]);

  const vbo = gl.createBuffer();
  const ebo = gl.createBuffer();

  gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
  gl.bufferData(gl.ARRAY_BUFFER, positions, gl.STATIC_DRAW);

  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, ebo);
  gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, indices, gl.STATIC_DRAW);

  const stride = 8 * 4;
  const positionLoc = gl.getAttribLocation(program, 'in_vert');
  const normalLoc = gl.getAttribLocation(program, 'in_normal');
  const texCoordLoc = gl.getAttribLocation(program, 'in_texCoord');

  gl.enableVertexAttribArray(positionLoc);
  gl.vertexAttribPointer(positionLoc, 3, gl.FLOAT, false, stride, 0);

  gl.enableVertexAttribArray(normalLoc);
  gl.vertexAttribPointer(normalLoc, 3, gl.FLOAT, false, stride, 3 * 4);

  gl.enableVertexAttribArray(texCoordLoc);
  gl.vertexAttribPointer(texCoordLoc, 2, gl.FLOAT, false, stride, 6 * 4);

  gl.bindVertexArray(null);
  return vao;
}

export function createFloorGeometry(gl, program) {
  const vao = gl.createVertexArray();
  gl.bindVertexArray(vao);

  const positions = new Float32Array([
    // positions         // normals       // texCoords
    -0.5, 0, -0.5,       0, 1, 0,         0, 0,
     0.5, 0, -0.5,       0, 1, 0,         1, 0,
     0.5, 0,  0.5,       0, 1, 0,         1, 1,
    -0.5, 0,  0.5,       0, 1, 0,         0, 1,
  ]);

  const indices = new Uint16Array([0, 1, 2, 0, 2, 3]);

  const vbo = gl.createBuffer();
  const ebo = gl.createBuffer();

  gl.bindBuffer(gl.ARRAY_BUFFER, vbo);
  gl.bufferData(gl.ARRAY_BUFFER, positions, gl.STATIC_DRAW);

  gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, ebo);
  gl.bufferData(gl.ELEMENT_ARRAY_BUFFER, indices, gl.STATIC_DRAW);

  const stride = 8 * 4;
  const positionLoc = gl.getAttribLocation(program, 'in_vert');
  const normalLoc = gl.getAttribLocation(program, 'in_normal');
  const texCoordLoc = gl.getAttribLocation(program, 'in_texCoord');

  gl.enableVertexAttribArray(positionLoc);
  gl.vertexAttribPointer(positionLoc, 3, gl.FLOAT, false, stride, 0);

  gl.enableVertexAttribArray(normalLoc);
  gl.vertexAttribPointer(normalLoc, 3, gl.FLOAT, false, stride, 3 * 4);

  gl.enableVertexAttribArray(texCoordLoc);
  gl.vertexAttribPointer(texCoordLoc, 2, gl.FLOAT, false, stride, 6 * 4);

  gl.bindVertexArray(null);
  return vao;
}
