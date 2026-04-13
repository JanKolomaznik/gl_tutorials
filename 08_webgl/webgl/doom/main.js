import { createGLContext, loadShaderProgram } from './gl-utils.js';
import { vertexShaderSrc, fragmentShaderEnemySrc, fragmentShaderFloorSrc } from './shaders.js';
import { initScene, drawScene, updateCamera, camera } from './scene.js';

const canvas = document.getElementById("glCanvas");
const gl = createGLContext(canvas);

gl.viewport(0, 0, canvas.width, canvas.height);
window.addEventListener('resize', () => {
  canvas.width = window.innerWidth;
  canvas.height = window.innerHeight;
  gl.viewport(0, 0, canvas.width, canvas.height);
});

const enemyProgram = loadShaderProgram(gl, vertexShaderSrc, fragmentShaderEnemySrc);
const floorProgram = loadShaderProgram(gl, vertexShaderSrc, fragmentShaderFloorSrc);

gl.enable(gl.DEPTH_TEST);
gl.enable(gl.BLEND);
gl.blendFunc(gl.SRC_ALPHA, gl.ONE_MINUS_SRC_ALPHA);

initScene(gl, enemyProgram, floorProgram);

canvas.requestPointerLock = canvas.requestPointerLock || canvas.mozRequestPointerLock;
canvas.onclick = () => canvas.requestPointerLock();

function render() {
  gl.viewport(0, 0, canvas.width, canvas.height);
  updateCamera();
  gl.clearColor(0.1, 0.1, 0.1, 1.0);
  gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
  drawScene(gl, enemyProgram, floorProgram);
  requestAnimationFrame(render);
}
requestAnimationFrame(render);
