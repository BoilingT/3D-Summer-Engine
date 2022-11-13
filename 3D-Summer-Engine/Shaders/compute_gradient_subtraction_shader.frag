#version 430 core
vec4 uNew;	//Out
uniform float frdx;		//0.5 / Gridscale
uniform sampler2D p;	//Pressure
uniform sampler2D w;	//Velocity

vec4 gradient(vec2 coords){
	//Pressure
	float pL = texture2D(p, coords - vec2(1, 0)).x;
	float pR = texture2D(p, coords + vec2(1, 0)).x;
	float pB = texture2D(p, coords - vec2(0, 1)).x;
	float pT = texture2D(p, coords + vec2(0, 1)).x;

	vec4 u = texture2D(w, coords);	//Velocity

	//Finite Difference Form of Gradient
	u -= frdx * vec2(pR - pL, pT - pB);
	return u;
}

void main(){
	vec2 coords = gl_FragCoord.xy;
	uNew = gradient(coords);
	gl_FragColor = uNew;
}