#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

// Uniform variables can be updated by fetching their location and passing values to that location
uniform mat4 projection;
uniform mat4 modelview;

out vec3 Normal;
out vec3 FragPos;
//out vec2 TexCoords;

void main()
{
    // OpenGL maintains the D matrix so you only need to multiply by P, V (aka C inverse), and M
    gl_Position = projection * modelview * vec4(position, 1.0f);
	Normal = normal;
	FragPos = vec3(modelview * vec4(position, 1.0f));
	//TexCoords = texCoords;
}
