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
	//splat = vec3(0.0f);

	//if(pow(coord.x - point.x/texelSize.x ,2) + pow(coord.y - point.y/texelSize.y,2) < 1000.0f){
	//	splat = vec3(p.x, p.y, 0.0f)*100.0f;
	//}
	
	//if(point.x > 0.5f){
	//	gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//}else{
	//	gl_FragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
	//}
	fragColor = vec4(base + splat, 1.0f);
	//if(base.x <= 0.3f){
	//	fragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);
	//}
}