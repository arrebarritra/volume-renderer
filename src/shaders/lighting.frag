#version 410 core
out vec4 FragColor;

in vec3 FragPos;
in vec2 TexCoords;
in mat3 TBN;

uniform sampler2D diffuse_map;
uniform sampler2D normal_map;
uniform sampler2D gloss_map;

uniform vec3 lightDir;
uniform vec3 viewPos;

void main(){
	vec3 viewDir = normalize(viewPos - FragPos);
	vec3 tangentNormal = normalize(texture(normal_map, TexCoords).xyz * 2.0 - 1.0);
	vec3 normal = TBN * tangentNormal;

	// diffuse
	float diffuse = max(dot(-lightDir, normal), 0.0);
	// specular
	vec3 halfwayDir = normalize(-lightDir + viewDir);
	float spec = max(dot(halfwayDir, normal), 0.0);
	spec *= pow(spec, texture(gloss_map, TexCoords).r);

	vec3 color = texture(diffuse_map, TexCoords).rgb;

	vec3 lighting = color * (diffuse);
	FragColor = vec4(lighting, 1.0); 
}