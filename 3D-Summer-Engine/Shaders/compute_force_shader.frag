#version 430 core

uniform sampler2D u; //Velocity field
uniform float x;	//Mouse pos x
uniform float y;	//Mouse pos y

//Calculate distance from impulse position of where the mouse clicked
void main(){
	vec2 pos = gl_FragCoord.xy;

	float F = 0;
	float pT = 0;
	float r = 1;
	float c = F*pT*exp((pow(x-pos.x,2) + pow(y-pos.y,2))/r);
	gl_FragColor = vec4(x, y, 0.0f, 1.0f);
}