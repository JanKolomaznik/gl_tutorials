# 01 TRIANGLE

## Overview

Rendering a single triangle is *hello world* of OpenGL rendering. 
The implementation shows the following steps:

 * Construction of application window using [GLFW](https://www.glfw.org/) library
 * Subsequent initialization of [GLAD](https://github.com/Dav1dde/glad) to load OGL functions
 * Define geometry and load it to GPU
 * Compile and link simple vertex and fragment shaders
 * Start rendering loop

### Vanilla demo

The `vanilla.cpp` source file is self contained (minus GLFW and glad) demo, where all the listed 
steps are in single file, mostly in *spaghetti* form. There is also limited error handling.

### Experiment

The `main.cpp` and the rest of the header files in this directory is our experimental code. 
The resource management is implemented by using *RAII* wrappers, code also employs the error checking tools from the utils dir.

You can compile several variants of the demo application by changing the include at the top 
of the file. Options are:
 * simpleTriangle.hpp
 * coloredTriangle.hpp
 * fractal.hpp

## Geometry definition

In OpenGL, geometry can be defined and rendered using various methods, each with its advantages and potential use cases. Here's an overview of the main ways to define and render a simple triangle in OpenGL, illustrating these methods with examples.

### Immediate Mode (Deprecated)
Immediate mode, also known as glBegin/glEnd mode, is the simplest but the least efficient way to define geometry. 
It's considered deprecated in modern OpenGL versions (OpenGL 3.x and above), but it's still useful for 
understanding basics or working with legacy code.

```cpp
glBegin(GL_TRIANGLES); 
    glVertex3f(-0.5f, -0.5f, 0.0f); // Vertex 1
    glVertex3f(0.5f, -0.5f, 0.0f);  // Vertex 2
    glVertex3f(0.0f, 0.5f, 0.0f);   // Vertex 3
glEnd();
```

### Vertex Arrays
Vertex arrays are a way to store vertex data in regular arrays and then pass them 
to OpenGL in a single batch, making rendering more efficient than immediate mode.

```cpp
float vertices[] = {
    -0.5f, -0.5f, 0.0f, // Vertex 1
    0.5f, -0.5f, 0.0f,  // Vertex 2
    0.0f, 0.5f, 0.0f    // Vertex 3
};

glEnableClientState(GL_VERTEX_ARRAY);
glVertexPointer(3, GL_FLOAT, 0, vertices);
glDrawArrays(GL_TRIANGLES, 0, 3);
glDisableClientState(GL_VERTEX_ARRAY);
```

### Vertex Buffer Objects (VBOs)
VBOs take the idea of vertex arrays further by storing the vertex data in the GPU's memory, 
which allows for even better performance, especially with large amounts of geometry.

```cpp
float vertices[] = {
    -0.5f, -0.5f, 0.0f, // Vertex 1
    0.5f, -0.5f, 0.0f,  // Vertex 2
    0.0f, 0.5f, 0.0f    // Vertex 3
};

GLuint VBO;
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glDrawArrays(GL_TRIANGLES, 0, 3);
glDisableVertexAttribArray(0);
```

### Vertex Array Objects (VAOs)
VAOs are used to store the state related to the layout of the vertex data, which includes VBOs. 
They make it easier to switch between different sets of vertex data and attribute configurations.

```cpp
float vertices[] = {
    -0.5f, -0.5f, 0.0f, // Vertex 1
    0.5f, -0.5f, 0.0f,  // Vertex 2
    0.0f, 0.5f, 0.0f    // Vertex 3
};

GLuint VBO, VAO;
glGenVertexArrays(1, &VAO);
glGenBuffers(1, &VBO);

glBindVertexArray(VAO);

glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

glBindVertexArray(0); // Unbind VAO

// When drawing
glBindVertexArray(VAO);
glDrawArrays(GL_TRIANGLES, 0, 3);
glBindVertexArray(0);
```

### Element Buffer Objects (EBOs) / Index Buffer Objects (IBOs)
EBOs, also known as IBOs, are used alongside VBOs to define which vertices form a primitive 
by indexing into the VBO. This method is efficient when vertices are shared among multiple primitives.

For a single triangle, using an EBO might not show its efficiency advantage, but it's included here for completeness.

```cpp
float vertices[] = {
    -0.5f, -0.5f, 0.0f, // Vertex 1
    0.5f, -0.5f, 0.0f,  // Vertex 2
    0.0f, 0.5f, 0.0f    // Vertex 3
};
unsigned int indices[] = {
    0, 1, 2 // Defining the triangle
};

GLuint VBO, VAO, EBO;
glGenVertexArrays(1, &VAO);
glGenBuffers(1, &VBO);
glGenBuffers(1, &EBO);

glBindVertexArray(VAO);

glBindBuffer(GL_ARRAY_BUFFER, VBO);
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

glBindVertexArray(0);

// When drawing
glBindVertexArray(VAO);
glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
glBindVertexArray(0);
```

Each of these methods has its place in OpenGL development, with the later ones (VBOs, VAOs, EBOs/IBOs) 
being the most relevant for modern, efficient OpenGL applications.
