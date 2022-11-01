#version 430 core

//in uvec3 gl_NumWorkGroups;
//in uvec3 gl_WorkGroupID;
//in uvec3 gl_LocalInvocationID;
//in uvec3 gl_GlobalInvocationID;
//in uint gl_LocalInvocationIndex;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D out_texture;


vec4 calculateValues(float val){
	return vec4((val), 1.0f, 0.0f, 0.0f);
}

void main(){
	//Get position to read/write data from/to
	ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

	//Get value stored in the texture (from the red channel)
	vec4 val = imageLoad(out_texture, texelCoord);
	//if(val.x <= 0.f && val.y <= 0.f){
	//	//val.x = float(texelCoord.x) / (gl_NumWorkGroups.x);
	//	//val.y = float(texelCoord.y) / (gl_NumWorkGroups.y);
	//	val.z = 1;
	//}

	//Store a new value in the texture
	imageStore(out_texture, texelCoord, val);
}