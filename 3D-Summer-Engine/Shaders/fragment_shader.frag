#version 430 core
out vec4 FragColor;
in vec4 vColor;

uniform vec4 color;

void main(){
	FragColor = vColor;
}