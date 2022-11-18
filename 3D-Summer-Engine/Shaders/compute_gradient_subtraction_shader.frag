#version 430 core
out vec4 fragColor;
uniform vec2 texelSize;		//0.5 / Gridscale
uniform sampler2D p;	//Pressure
uniform sampler2D w;	//Velocity

//Calculate
vec2 gradient(vec2 coords){
	vec2 vL = coords - vec2(1, 0);
	vec2 vR = coords + vec2(1, 0);
	vec2 vB = coords - vec2(0, 1);
	vec2 vT = coords + vec2(0, 1);

	//Pressure
	float pL = texture2D(p, vL * texelSize).x;
	float pR = texture2D(p, vR * texelSize).x;
	float pB = texture2D(p, vB * texelSize).x;
	float pT = texture2D(p, vT * texelSize).x;

	vec4 u = texture2D(w, coords * texelSize);	//Velocity with divergence

	//Finite Difference Form of Gradient
	u.xy = u.xy + texelSize * 0.5f - vec2(pR - pL, pT - pB);
	return u;	//Velocity without divergence
}

void main(){
	vec2 coords = gl_FragCoord.xy;
	vec2 result = gradient(coords);
	fragColor = vec4(result, 0.0f, 1.0f);
	//gl_FragColor = vec4(coords/16.0f, 0.0f, 1.0f);
}