#version 430
out vec4 fragColor;
uniform vec2 texelSize;
uniform sampler2D uTexture;
uniform float value;

void main () {
    fragColor = texture2D(uTexture, gl_FragCoord.xy * texelSize);
}