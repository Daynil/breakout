#shader vertex
#version 330 core

// GLSL is the opengl shading language
// gl_Position must be a vec4, and since we told OpenGL we are passing in a vec3, we do so here
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec2 aTexCoord;

//out vec3 ourColor;
out vec2 texCoord;

uniform mat4 transform;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    // This is a predefined variable by OpenGL
    //gl_Position = transform * vec4(aPosition, 1.0);
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
    //gl_Position = vec4(position.x, -position.y, position.z, 1.0);
    //ourColor = aColor;
    texCoord = aTexCoord;
};

#shader fragment
#version 330 core

out vec4 fragColor;

//in vec3 ourColor;
in vec2 texCoord;

uniform sampler2D ourTexture;

void main()
{
    // Colors in graphics programming are RGBA floats between 0 and 1
    //color = vec4(0.1, 0.5, 0.1, 1.0);
    //fragColor = ourColor;
    fragColor = texture(ourTexture, texCoord); // * vec4(ourColor, 1.0);
};