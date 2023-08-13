#version 430 core
#define OFFSET 0.5
in vec2 vUv;
out vec4 fragColor;
float div;	//Out
uniform vec2 texelSize;		//0.5 / Gridscale
uniform sampler2D w;	//Velocity field

//Calculate change in density of a fluid
float divergence(vec2 coords){
	float offset = OFFSET;
	vec2 vL = coords - vec2(1, 0) * texelSize;
	vec2 vR = coords + vec2(1, 0) * texelSize;
	vec2 vB = coords - vec2(0, 1) * texelSize;
	vec2 vT = coords + vec2(0, 1) * texelSize;

	float wL = texture2D(w, vL).x;
	float wR = texture2D(w, vR).x;
	float wB = texture2D(w, vB).y;
	float wT = texture2D(w, vT).y;

	//Boundary conditions
	vec2 u = texture2D(w, coords).xy;
	if (vL.x < 0.0f) { wL = -u.x; }
    if (vR.x > 1.0f) { wR = -u.x; }
    if (vT.y > 1.0f) { wT = -u.y; }
    if (vB.y < 0.0f) { wB = -u.y; }

	//Finite Difference Form of Divergence
	//return u;
	//Divergence = 0.5f * w
	return 0.5f * (wR - wL + wT - wB);
}

void main(){
	//0 - WIDTH (700) * 1/16
	vec2 coords = vUv;
	//coord => 0 - 16
	div = divergence(coords);
	fragColor = vec4(div, 0.0f, 0.0f, 1.0f);
}