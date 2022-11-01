#version 430 core

// 1 pixel per workgroup
layout (local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout (rgba32f, binding = 0) uniform image2D imgOutput;

layout(location = 0) uniform float t;
layout(location = 1) uniform float textureWidth;

void main(){
	//vec4 val = vec4(0.0f, 0.0f, 0.0f, 1.0f);
	ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
	vec4 val = imageLoad(imgOutput, texelCoord);
	float distance = 5.f;

	float speed = 10;
	if(float(texelCoord.y) > textureWidth/2.f){
		//val.y = float(texelCoord.y + sin(t) * speed) / (gl_NumWorkGroups.y);
	}else{
		//val.y = float(texelCoord.y + sin(-t) * speed) / (gl_NumWorkGroups.y);
	}
	//val.x = float(texelCoord.x) / (gl_NumWorkGroups.x);

	ivec2 texelOffset = ivec2(1, 0);

	imageStore(imgOutput, texelCoord, val);
}