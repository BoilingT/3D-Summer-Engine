#version 430 core
#define mode 0
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 1) in vec2 aOffset;

out vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec2 texelSize;

void main(){
	mat4 clip_space = projection * view * model;
	texCoord = aTexCoord;

	gl_Position = clip_space * vec4(aPos.x + aOffset.x, aPos.y + aOffset.y, aPos.z, 1.0f);
	//gl_Position = vec4(aPos.x + aOffset.x, aPos.y + aOffset.y, aPos.z, 1.0f);
}