#version 430 core
#define OFFSET 0.5
out vec4 fragColor;
float div;	//Out
uniform vec2 texelSize;		//0.5 / Gridscale
uniform sampler2D w;	//Vector field

//Calculate change in density of a fluid
float divergence(vec2 coords){
	float offset = OFFSET;
	//0 - 16
	vec2 vL = (coords - vec2(1, 0));
	vec2 vR = (coords + vec2(1, 0));
	vec2 vB = (coords - vec2(0, 1));
	vec2 vT = (coords + vec2(0, 1));

	//0 - 1
	float wL = texture2D(w, vL * texelSize).x;
	float wR = texture2D(w, vR * texelSize).x;
	float wB = texture2D(w, vB * texelSize).y;
	float wT = texture2D(w, vT * texelSize).y;

	vec2 u = texture2D(w, coords*texelSize).xy;
	if (vL.x * texelSize.x < 0.0f) { wL = -u.x; }
    if (vR.x * texelSize.x > 1.0f) { wR = -u.x; }
    if (vT.y * texelSize.y > 1.0f) { wT = -u.y; }
    if (vB.y * texelSize.y < 0.0f) { wB = -u.y; }

	//Finite Difference Form of Divergence
	//return u;
	//Divergence = 0.5f * w
	return 0.5f * (wR - wL + wT - wB);
}

void main(){
	//0 - WIDTH (700) * 1/16
	vec2 coords = gl_FragCoord.xy;
	//coord => 0 - 16
	div = divergence(coords);
	fragColor = vec4(div, 0.0f, 0.0f, 1.0f);
}