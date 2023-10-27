#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aTexCoords;

out vec3 FragPos;
out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = gl_Position.xyz;
	TexCoords = aTexCoords;
}