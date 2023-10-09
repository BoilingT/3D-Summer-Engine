#version 430
in vec2 vUv;
out vec4 fragColor;
uniform sampler2D uTarget;
uniform vec3 color;
uniform vec2 point;
uniform float radius;
uniform vec2 texelSize;
uniform int amount;

float calcSplat(vec2 pos, float r, vec2 coord, float ratio){
	vec2 p = coord - pos;
	p*=ratio;
	return exp(-dot(p, p) / (r*0.03f));
}

float getFinalSplat(vec2 coord, float ratio){
	float splat = 0.0f;
	if(amount <= 1){
		splat = calcSplat(point, radius, coord, ratio);
	}else if(amount > 1){
		for(int i = 0; i < amount; i++){
			splat += calcSplat(vec2((1.0f) / amount * ((i + 1.0f)) - ((1.0f) / amount / 2.0f), point.y), radius, coord, ratio);
		}
	}
	return splat;
}

void main(){
	//0 - 256 Dye
	//0 - 16  Velocity
	
	//vec3 base = splat(vec2(0.5f, 0.5f), 0.25f, vec3(1.0f), vec3(0.0));
	
	float ratio = 1.0f / (texelSize.x / texelSize.y);
		
	vec3 base = texture2D(uTarget, vUv).xyz;

	fragColor = vec4(base + color * getFinalSplat(vUv, ratio), 1.0f);

	/*vec2 p = vUv - point.xy;
	p.x *= 1920.0f / 1080.0f;
	vec3 splat = exp(-dot(p, p) / (radius / 100.0f)) * color;
	vec3 base = texture2D(uTarget, vUv).xyz;
	fragColor = vec4(base + splat, 1.0); */
}