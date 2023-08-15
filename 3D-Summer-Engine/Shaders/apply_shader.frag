#version 430
in vec2 vUv;
out vec4 fragColor;
uniform sampler2D uTexture;
uniform vec4 value;

void main () {
	vec4 u = texture2D(uTexture, vUv);
	fragColor = u + value;
}