import { mat4, mat3, vec3 } from "https://cdn.jsdelivr.net/npm/gl-matrix@3.4.3/esm/index.js";
import { createEnemyGeometry, createFloorGeometry } from './geometry.js';
import { loadTexture } from './gl-utils.js';

export let camera = {
	pos: vec3.fromValues(0, 0.75, 5),
	front: vec3.fromValues(0, 0, -1),
	up: vec3.fromValues(0, 1, 0),
	yaw: -90,
	pitch: 0,
	speed: 0.1,
};

let keys = {};
window.addEventListener('keydown', e => keys[e.key] = true);
window.addEventListener('keyup', e => keys[e.key] = false);
document.addEventListener('mousemove', e => {
	if (document.pointerLockElement === document.querySelector('canvas')) {
		const sensitivity = 0.1;
		camera.yaw += e.movementX * sensitivity;
		camera.pitch -= e.movementY * sensitivity;
		camera.pitch = Math.max(-89, Math.min(89, camera.pitch));
		const front = [
			Math.cos(degToRad(camera.yaw)) * Math.cos(degToRad(camera.pitch)),
			0,
			Math.sin(degToRad(camera.yaw)) * Math.cos(degToRad(camera.pitch))
		];
		vec3.normalize(camera.front, front);
	}
});

function degToRad(d) {
	return d * Math.PI / 180;
}

export function updateCamera() {
	const right = vec3.create();
	vec3.cross(right, camera.front, camera.up);
	vec3.normalize(right, right);
	let move = vec3.create();
	if (keys['w']) vec3.scaleAndAdd(camera.pos, camera.pos, camera.front, camera.speed);
	if (keys['s']) vec3.scaleAndAdd(camera.pos, camera.pos, camera.front, -camera.speed);
	if (keys['a']) vec3.scaleAndAdd(camera.pos, camera.pos, right, -camera.speed);
	if (keys['d']) vec3.scaleAndAdd(camera.pos, camera.pos, right, camera.speed);
}

export const enemyPositions = [
	[5, 0, -2],
	[15, 0, -15],
	[-2, 0, -12],
	[-23, 0, -9],
	[0, 0, -6]
];

let enemyVAO, floorVAO;
let floorTexture, enemyTexture;
export function initScene(gl, enemyProgram, floorProgram) {
	enemyVAO = createEnemyGeometry(gl, enemyProgram);
	floorVAO = createFloorGeometry(gl, floorProgram);

	floorTexture = loadTexture(gl, './floor.png');
	enemyTexture = loadTexture(gl, './imp.png');
}

function renderEnemies(gl, program, view, proj) {
  const normalMat = mat3.create();
  const uModel = gl.getUniformLocation(program, 'u_modelMat');
  const uView = gl.getUniformLocation(program, 'u_viewMat');
  const uProj = gl.getUniformLocation(program, 'u_projMat');
  const uNormal = gl.getUniformLocation(program, 'u_normalMat');

  gl.useProgram(program);
  gl.uniformMatrix4fv(uView, false, view);
  gl.uniformMatrix4fv(uProj, false, proj);

  gl.activeTexture(gl.TEXTURE0);
  gl.bindTexture(gl.TEXTURE_2D, enemyTexture);
  gl.uniform1i(gl.getUniformLocation(program, 'u_texture'), 0);

  gl.bindVertexArray(enemyVAO);
  for (const pos of enemyPositions) {
    const model = mat4.create();
    mat4.translate(model, model, pos);
    mat3.fromMat4(normalMat, model);
    mat3.invert(normalMat, normalMat);
    mat3.transpose(normalMat, normalMat);
    gl.uniformMatrix4fv(uModel, false, model);
    gl.uniformMatrix3fv(uNormal, false, normalMat);
    gl.drawElements(gl.TRIANGLES, 6, gl.UNSIGNED_SHORT, 0);
  }
}

function renderFloor(gl, program, view, proj) {
  const normalMat = mat3.create();
  const uModel = gl.getUniformLocation(program, 'u_modelMat');
  const uView = gl.getUniformLocation(program, 'u_viewMat');
  const uProj = gl.getUniformLocation(program, 'u_projMat');
  const uNormal = gl.getUniformLocation(program, 'u_normalMat');

  gl.useProgram(program);
  gl.uniformMatrix4fv(uView, false, view);
  gl.uniformMatrix4fv(uProj, false, proj);

  const model = mat4.create();
  mat4.scale(model, model, [100, 1, 100]);
  mat3.fromMat4(normalMat, model);
  mat3.invert(normalMat, normalMat);
  mat3.transpose(normalMat, normalMat);

  gl.uniformMatrix4fv(uModel, false, model);
  gl.uniformMatrix3fv(uNormal, false, normalMat);

  gl.activeTexture(gl.TEXTURE0);
  gl.bindTexture(gl.TEXTURE_2D, floorTexture);
  gl.uniform1i(gl.getUniformLocation(program, 'u_texture'), 0);

  gl.bindVertexArray(floorVAO);
  gl.drawElements(gl.TRIANGLES, 6, gl.UNSIGNED_SHORT, 0);
}

export function drawScene(gl, enemyProgram, floorProgram) {

	const view = mat4.create();
	const center = vec3.create();
	vec3.add(center, camera.pos, camera.front);
	mat4.lookAt(view, camera.pos, center, camera.up);

	const proj = mat4.create();
	mat4.perspective(proj, Math.PI / 4, gl.canvas.width / gl.canvas.height, 0.1, 100);

	renderFloor(gl, floorProgram, view, proj);

	renderEnemies(gl, enemyProgram, view, proj);
}
