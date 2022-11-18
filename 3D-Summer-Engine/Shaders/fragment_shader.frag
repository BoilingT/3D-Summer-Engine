#version 430 core
#define WHITE vec4(1.0f);

in vec4 vColor;
varying vec2 vUv;
in vec2 texCoord;
uniform sampler2D u_image;

void main(){
		gl_FragColor = texture(u_image, vUv);
}