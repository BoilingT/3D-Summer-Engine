#version 430
uniform vec2 texelSize;
uniform sampler2D uTexture;
uniform float value;

void main () {
    gl_FragColor = value * texture2D(uTexture, gl_FragCoord.xy * texelSize);
}