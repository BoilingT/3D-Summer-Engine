#version 430 core

out vec4 uNew;
uniform float frdx;		//0.5 / Gridscale
uniform sampler2D p;	//Pressure
uniform sampler2D w;	//Velocity

void gradient(vec2 coords){
	//Pressure
	float pL = texture2D(p, coords - vec2(1, 0)).x;
	float pR = texture2D(p, coords + vec2(1, 0)).x;
	float pB = texture2D(p, coords - vec2(0, 1)).x;
	float pT = texture2D(p, coords + vec2(0, 1)).x;

	uNew = texture2D(w, coords);	//Velocity

	uNew.xy -= frdx * vec2(pR - pL, pT - pB);
}

void main(){
	vec2 coords = gl_FragCoord.xy;
	gradient(coords);
}