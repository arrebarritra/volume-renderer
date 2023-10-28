#version 410 core
out vec4 FragColor;

in vec3 TexCoords;

uniform usampler3D volume;

void main(){
    float alpha = texture(volume, TexCoords).r;
    FragColor = vec4(vec3(1.0f), alpha);
}

