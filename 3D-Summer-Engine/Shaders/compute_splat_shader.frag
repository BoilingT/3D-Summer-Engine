#version 430
in vec2 vUv;
out vec4 fragColor;
uniform sampler2D uTarget;
uniform vec3 color;
uniform vec2 point;
uniform float radius;
uniform vec2 texelSize;

void main(){
	//0 - 256 Dye
	//0 - 16  Velocity
	float ratio = 1.0f / (texelSize.x / texelSize.y);
	vec2 coord = gl_FragCoord.xy;
	vec2 p = coord * texelSize - point;
	p*=ratio;
	vec3 splat = exp(-dot(p, p) / (radius/100.0f)) * color;
	vec3 base = texture2D(uTarget, coord * texelSize).xyz;
	//if(point.x > 0.5f){
	//	gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//}else{
	//	gl_FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	//}
	fragColor = vec4(base + splat, 1.0f);
}