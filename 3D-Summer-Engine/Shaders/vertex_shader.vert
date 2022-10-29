#version 430 core
#define mode 0
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 1) in vec2 aOffset;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform int resolution;
uniform float u_time;

out vec2 texCoord;
out float time;
out float res;

void main(){
	mat4 clip_space = projection * view * model;
	int o = 1;
	float ID = gl_InstanceID;
	float c = aOffset.x;
	float r = aOffset.y;
	float k = sqrt(resolution);
	time = u_time;
	texCoord = aTexCoord;
	res = resolution;
	gl_Position = clip_space * vec4(aPos.x + aOffset.x, aPos.y + aOffset.y, aPos.z, 1.0f);
}