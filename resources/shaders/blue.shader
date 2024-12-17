#shader vertex
#version 330 core

// GLSL is the opengl shading language
// gl_Position must be a vec4, and since we told OpenGL we are passing in a vec3, we do so here
layout(location = 0) in vec3 position;
layout(location = 1) in vec3 ourColor;

out vec3 color;

uniform float horizontalOffset;

void main()
{
    // This is a predefined variable by OpenGL
    gl_Position = vec4(position.x + horizontalOffset, position.y, position.z, 1.0);
    //color = ourColor;
    color = vec3(position.x + horizontalOffset, position.y, position.z);
};

#shader fragment
#version 330 core

in vec3 color;

out vec4 fragColor;

void main()
{
    // Colors in graphics programming are RGBA floats between 0 and 1
    fragColor = vec4(color, 1.0f);
};