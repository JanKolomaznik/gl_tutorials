# gl_tutorials
Short demos and tutorials for OpenGL and other graphic libraries.

## Dependencies

### Loading library

To use modern OpenGL you need [loading library](https://www.khronos.org/opengl/wiki/OpenGL_Loading_Library), which will set the function pointers for the API calls and load vendor extensions.

There is several options for such libraries. I chose [GLAD](https://github.com/Dav1dde/glad) as it should be sufficient
for our purposes. I generated basic version of the lib for the OpenGL 4.4 and it is stored in this repository in the `glad` directory, so there is no need for additional steps.


#### Why simple GL.h include does not suffice

Limited to OpenGL's Core Version: GL.h typically corresponds to OpenGL version 1.1, which is severely outdated. 
It doesn't include function pointers for modern OpenGL features or extensions.

Platform-Specific Function Pointers: OpenGL's design requires that function pointers for newer features be obtained at runtime. 
This is because different graphics drivers may implement different versions of OpenGL and support different sets of extensions. 
A static header file like GL.h cannot provide this level of flexibility.

## Windowing library

OpenGL requires either native window or window created by specialized library, which will provide OpenGL context and framebuffer.
Basic multiplatform solution is [GLFW](https://www.glfw.org/).

## GLM

[GLM](https://github.com/g-truc/glm) provides linear algebra library with similar syntax to GLSL. Download [here](https://github.com/g-truc/glm/releases/tag/1.0.1).
