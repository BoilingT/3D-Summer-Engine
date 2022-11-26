#version 430

out vec4 outColor;

uniform sampler2D u; //Velocities
uniform vec2 texelSize;
uniform float dt;

vec4 curl(vec2 coord, sampler2D v){
	vec4 xL = texture2D(v, coord - vec2(1, 0));
	vec4 xR = texture2D(v, coord + vec2(1, 0));
	vec4 yT = texture2D(v, coord + vec2(0, 1));
	vec4 yB = texture2D(v, coord - vec2(0, 1));

	return yT - yB + xR - xL;
}

vec4 vorticityConfinement(vec2 coord, vec4 curl){
	vec2 v = texture2D(u, coord * texelSize).xy;

	//Vorticity
	//Vorticity, ? = cross(x, c);

	//Compute Vorticity: omega = cross(x, v);
	//n = nabla |omega|; //Vorticity
	//phi = n / |n|; //Normalized vorticity
	//phi = (del |omega|)/(|del |omega|);

	//Compute force
	//force = epsilon(phi x vorticity)
	//float force = epsilon(cross(phi, omega)) * dt;

	return vec4(0.0f);
}

void main(){
	vec2 coords = gl_FragCoord.xy * texelSize;

	vec4 curlResult = curl(coords, u);
	vec4 result = vorticityConfinement(coords, curlResult);
	outColor = result;
}