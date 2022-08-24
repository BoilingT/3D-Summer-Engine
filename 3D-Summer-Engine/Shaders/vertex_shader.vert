#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 rgbValues;
layout (location = 2) in vec2 textureCoordValues;

out vec3 vertexPos;
out vec3 colorValues;
out vec2 textureCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){
	mat4 clip_space = projection * view * model;
	gl_Position = clip_space * vec4(aPos, 1.0f);
	vertexPos = aPos;
	colorValues = rgbValues;
	textureCoord = textureCoordValues;
}