#version 430
out vec4 outColor;

uniform sampler2D u; //Velocities
uniform vec2 texelSize;
uniform float dt;

vec4 curl(vec2 coord, sampler2D v){
	vec4 L = texture2D(v, (coord - vec2(1, 0)) * texelSize);
	vec4 R = texture2D(v, (coord + vec2(1, 0)) * texelSize);
	vec4 T = texture2D(v, (coord + vec2(0, 1)) * texelSize);
	vec4 B = texture2D(v, (coord - vec2(0, 1)) * texelSize);
	vec4 c = texture2D(v, coord * texelSize);
	//return T - B + R - L; //Curl
	
	//return R - L - T + B; //Curl
	return 0.5f * ((R-L) - (T-B));
}

void main(){
	//0 - 256
	//x * tx.x => 0 - 1
	vec2 coord = gl_FragCoord.xy;
	vec4 result = curl(coord, u);
	outColor = vec4(result.x, 0.0f, 0.0f, 1.0f);
}