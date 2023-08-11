#version 430
in vec2 vUv;
out vec4 fragColor;
uniform vec2 texelSize;
uniform sampler2D uTexture;
uniform float value;

void main () {
    fragColor = value * texture2D(uTexture, vUv);
}