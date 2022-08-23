#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 rgbValues;
layout (location = 2) in vec2 textureCoordValues;

out vec3 vertexPos;
out vec3 colorValues;
out vec2 textureCoord;

uniform mat4 transform;

void main(){
	gl_Position = transform * vec4(aPos.x, aPos.y, aPos.z, 1.0f);
	vertexPos = aPos;
	colorValues = rgbValues;
	textureCoord = textureCoordValues;
}