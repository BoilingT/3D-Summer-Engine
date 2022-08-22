#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 rgbValues;
layout (location = 2) in vec2 textureCoordValues;

out vec2 textureCoord;

uniform vec4 posOffset;

void main(){
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f) + posOffset;
	textureCoord = textureCoordValues;
}