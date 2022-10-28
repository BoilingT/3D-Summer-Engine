#version 430 core
#define WHITE vec4(1.0f);
out vec4 FragColor;
in vec4 vColor;

uniform sampler2D u_image;

void main(){
	FragColor = WHITE;
}