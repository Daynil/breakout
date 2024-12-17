#shader vertex
#version 330 core

// GLSL is the opengl shading language
// gl_Position must be a vec4, and since we told OpenGL we are passing in a vec3, we do so here
layout(location = 0) in vec3 position;

void main()
{
    // This is a predefined variable by OpenGL
    gl_Position = vec4(position.x, position.y, position.z, 1.0);
    //gl_Position = vec4(position.x, -position.y, position.z, 1.0);
};

#shader fragment
#version 330 core

out vec4 color;

uniform vec4 ourColor;

void main()
{
    // Colors in graphics programming are RGBA floats between 0 and 1
    //color = vec4(0.1, 0.5, 0.1, 1.0);
    color = ourColor;
};