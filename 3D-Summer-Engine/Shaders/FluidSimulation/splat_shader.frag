#version 430
in vec2 vUv;
out vec4 fragColor;
uniform sampler2D uTarget;
uniform vec3 color;
uniform vec3 velocities;
uniform vec2 point;
uniform float radius;
uniform vec2 texelSize;
uniform int amount;

float calcSplat(vec2 pos, float r, vec2 coord, float ratio){
	vec2 p = coord - pos;
	p*=ratio;
	//float value = 0;
	//if(pow(vUv.x - pos.x,2) + pow(vUv.y - pos.y,2) <= r*0.03f) value = 1;
	float value = exp(-dot(p, p) / (r*0.03f));
	//float value = exp(-(pow(vUv.x - pos.x,2) + pow(vUv.y - pos.y,2)) / (r*0.03f));
	/*float thickness = 1/10000.0f;
	if(pow(vUv.x - pos.x,2) + pow(vUv.y - pos.y,2) >= r*0.03f-thickness) value = 0;*/
	return value;
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

	if(length(color) > 0 && length(velocities) <= 0){
		vec3 splatValue = color * getFinalSplat(vUv, ratio);

		//if(base.r >= splatValue.r){splatValue.r = 0;}
		//if(base.g >= splatValue.g){splatValue.g = 0;}
		//if(base.b >= splatValue.b){splatValue.b = 0;}
		
		vec3 value = base + splatValue;
		/*vec2 p = vUv - point;
		p *= ratio;
		float thickness = 1/10000.0f;
		float r = radius;
		//if(dot(p, p) >= r*0.03f - thickness && dot(p, p) <= r*0.03f + thickness) value += vec3(0.01f,0,0);
		if(amount <= 1){
			if(pow(vUv.x - point.x,2) + pow(vUv.y - point.y,2) >= r*0.03f-thickness && pow(vUv.x - point.x,2) + pow(vUv.y - point.y,2) <= r*0.03f+thickness) value += vec3(0.1f);
		}else{
			for(int i = 0; i < amount; i++){
				vec2 temp = vec2((1.0f) / amount * ((i + 1.0f)) - ((1.0f) / amount / 2.0f), point.y);
				if(pow(vUv.x - temp.x,2) + pow(vUv.y - temp.y,2) >= r*0.03f-thickness && pow(vUv.x - temp.x,2) + pow(vUv.y - temp.y,2) <= r*0.03f+thickness) value += vec3(0.01f);
			}		
		}*/
		fragColor = vec4(value, 1.0f);
	}
	else {
		vec3 splatValue = velocities * getFinalSplat(vUv, ratio);
		vec3 value = base + splatValue;
		fragColor = vec4(value, 1.0f);
	}
	/*vec2 p = vUv - point.xy;
	p.x *= 1920.0f / 1080.0f;
	vec3 splat = exp(-dot(p, p) / (radius / 100.0f)) * color;
	vec3 base = texture2D(uTarget, vUv).xyz;
	fragColor = vec4(base + splat, 1.0); */
}