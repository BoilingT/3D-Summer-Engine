#version 430 core

//in uvec3 gl_NumWorkGroups;
//in uvec3 gl_WorkGroupID;
//in uvec3 gl_LocalInvocationID;
//in uvec3 gl_GlobalInvocationID;
//in uint gl_LocalInvocationIndex;

layout(local_size_x = 1, local_size_y = 1, local_size_z = 1) in;
layout(r32f, binding = 0) uniform image2D out_texture;

void main(){
	//Get position to read/write data from/to
	ivec2 pos = ivec2(gl_GlobalInvocationID.xy);

	//Get value stored in the texture (from the red channel)
	float texture_val = imageLoad(out_texture, pos).r;

	//Store a new value in the texture
	imageStore(out_texture, pos, vec4(texture_val + 1.0f, 0.0f, 0.0f, 0.0f));
}