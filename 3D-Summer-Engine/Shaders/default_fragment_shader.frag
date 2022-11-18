#version 430 core
out vec4 fragColor;

in vec2 textureCoord;

uniform vec4 ourColor;
uniform sampler2D ourTexture;

void main(){
	fragColor = texture(ourTexture, textureCoord);
}