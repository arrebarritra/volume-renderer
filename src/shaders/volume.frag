#version 410 core
out vec4 FragColor;

in vec3 TexCoords;

uniform sampler3D volume;
uniform float alphascale;

void main(){
    float alpha = texture(volume, TexCoords).r * alphascale;
    FragColor = vec4(vec3(1.0f), alpha);
}
