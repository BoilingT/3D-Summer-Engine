#version 430 core
#define mode 1
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
	float ID = gl_InstanceID;
	float c = aOffset.x;
	float r = aOffset.y;
	float k = sqrt(resolution);
	#if (mode == 1)
	vColor = vec4((c+1.f) /k, 0.0f, (r+1.f) /k, 1.0f);
	#elif (mode == 2)
	//v = r*w+c
	//ID increments for every row thus adding the rowindex the sum will alternate between being even and noneven
	if(int(ID+1+r) % 2 == 0){
		vColor = vec4(vec3(1.0f), 1.0f);
	}else{
		vColor = vec4(vec3(0.0f), 1.0f);
	}
	//vColor = vec4(float(ID)/float(resolution), 0.0f, 0.0f, 1.0f);
	#endif
	gl_Position = clip_space * vec4(aPos.x + aOffset.x, aPos.y + aOffset.y, aPos.z, 1.0f);
}