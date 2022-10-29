#version 430 core
#define WHITE vec4(1.0f);
out vec4 FragColor;

in vec4 vColor;
in vec2 texCoord;

uniform sampler2D u_image;

void main(){
	FragColor = texture(u_image, texCoord);
}