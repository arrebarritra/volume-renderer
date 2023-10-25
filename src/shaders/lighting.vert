#version 410 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

out vec3 FragPos;
out vec2 TexCoords;
out mat3 TBN;

void main()
{
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	FragPos = gl_Position.xyz;

	TexCoords = aTexCoords;

	mat3 normalMatrix =  transpose(inverse(mat3(model)));
	vec3 vNormal = normalMatrix * aNormal;
	vec3 vTangent = normalMatrix * aTangent;
	vec3 vBitangent = normalMatrix * aBitangent;
	TBN = mat3(vTangent, vBitangent, vNormal); //tangent space to world space
}