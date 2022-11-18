#version 430 core
#define WHITE vec4(1.0f);

out vec4 fragColor;
in vec4 vColor;
varying vec2 vUv;
in vec2 texCoord;
uniform sampler2D u_image;

void main(){
		fragColor = texture(u_image, vUv);
}