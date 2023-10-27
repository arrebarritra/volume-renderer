#version 410 core

out vec4 FragColor;

in vec3 FragPos;
in vec3 TexCoords;

uniform usampler3D volume;

void main(){
    float alpha = texture(volume, vec3(TexCoords.x, TexCoords.y, FragPos.z)).r;
    FragColor = vec4(vec3(1.0f), alpha);
}

