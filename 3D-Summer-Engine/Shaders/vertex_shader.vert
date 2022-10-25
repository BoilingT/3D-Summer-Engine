#version 430 core
layout (location = 0) in vec3 aPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main(){
	mat4 clip_space = projection * view * model;
	gl_Position = clip_space * vec4(aPos, 1.0f);
}