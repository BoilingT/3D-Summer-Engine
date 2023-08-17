#version 430 core
in vec2 vUv;
out vec4 fragColor;
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
	vec3 base = texture2D(u, vUv).xyz;
	fragColor = vec4(base + c, 1.0f);
}