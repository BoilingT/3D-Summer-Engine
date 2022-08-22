#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 rgbValues;

out vec3 vertexPos;
out vec3 colorValues;

void main(){
	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);
	vertexPos = aPos;
	colorValues = rgbValues;
}