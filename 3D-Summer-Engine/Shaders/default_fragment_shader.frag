#version 430 core
out vec4 FragColor;

in vec2 textureCoord;

uniform vec4 ourColor;
uniform sampler2D ourTexture;

void main(){
	FragColor = texture(ourTexture, textureCoord);
}