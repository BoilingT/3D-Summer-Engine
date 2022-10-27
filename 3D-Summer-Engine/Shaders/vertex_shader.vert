#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aOffset;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform int resolution;

//uniform vec2 offsets[32*32];
out vec4 vColor;

void main(){
	mat4 clip_space = projection * view * model;
	int o = 1;
	int ID = gl_InstanceID;
	float c = aOffset.x;
	float r = aOffset.y;
	float k = sqrt(resolution);
	#if 1
	if(ID % 2 == 0){
		vColor = vec4(0.5f, 0.5f, 0.5f, 1.0f);	
	}else{
		vColor = vec4(1.0f);
	}
	#else
		vColor = vec4((c+1.f) /k, 0.0f, (r+1.f) /k, 1.0f);	
	#endif
	gl_Position = clip_space * vec4(aPos.x + aOffset.x, aPos.y + aOffset.y, aPos.z, 1.0f);
}