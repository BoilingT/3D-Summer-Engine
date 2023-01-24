#version 430 core
out vec4 fragColor;

uniform float timestep;
uniform float scale;
uniform vec2 texelSize;
uniform vec2 off;
uniform sampler2D u;	//Input velocity

vec4 bounds(vec2 coords, vec2 offset){

	return scale * texture2D(u, (coords + offset) * texelSize);
}

void main(){
	vec2 coords = gl_FragCoord.xy;
	vec4 result = bounds(coords, off);
	//fragColor = result;
	fragColor = vec4(scale);
}