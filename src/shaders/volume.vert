#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aTexCoords;

out vec3 FragPos;
out vec3 TexCoords;

uniform mat4 projection;

void main()
{
	TexCoords = aTexCoords;
	gl_Position = projection * vec4(aPos, 1.0);
}