#version 430 core
out vec4 div;
uniform float frdx;		//0.5 / Gridscale
uniform sampler2D w;	//Vector field

void divergence(vec2 coords){
	vec4 wL = texture2D(w, coords - vec2(1, 0));
	vec4 wR = texture2D(w, coords + vec2(1, 0));
	vec4 wB = texture2D(w, coords - vec2(0, 1));
	vec4 wT = texture2D(w, coords + vec2(0, 1));

	div = frdx * ((wR - wL) + (wT - wB));
}

void main(){
	vec2 coords = gl_FragCoord.xy;
	divergence(coords);
}