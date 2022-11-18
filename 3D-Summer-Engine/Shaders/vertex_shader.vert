#version 430 core
#define mode 0
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoord;
layout (location = 1) in vec2 aOffset;

out vec2 texCoord;
varying vec2 vUv;

out vec2 vL;
out vec2 vR;
out vec2 vT;
out vec2 vB;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform vec2 texelSize;



void main(){
	mat4 clip_space = projection * view * model;
	texCoord = aTexCoord;
	vUv = aPos.xy * 0.5f + 0.5f;
	vL	= vUv - vec2(texelSize.x, 0.0f);
	vR	= vUv + vec2(texelSize.x, 0.0f);
	vT	= vUv + vec2(0.0f, texelSize.y);
	vB	= vUv - vec2(0.0f, texelSize.y);

	//gl_Position = clip_space * vec4(aPos.x + aOffset.x, aPos.y + aOffset.y, aPos.z, 1.0f);
	gl_Position = vec4(aPos.x + aOffset.x, aPos.y + aOffset.y, aPos.z, 1.0f);
}