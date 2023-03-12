#version 430 core
#define WHITE vec4(1.0f);

out vec4 fragColor;
in vec4 vColor;
in vec2 vUv;
in vec2 texCoord;

uniform sampler2D u_image;
uniform sampler2D u_image_overlay;
uniform vec2 texelSize;
uniform int scene;

vec4 getSciColor(float value, float minVal, float maxVal) {
		float val = min(max(value, minVal), maxVal- 0.0001f);
		float d = maxVal - minVal;
		val = d == 0.0f ? 0.5f : (value - minVal) / d;
		float m = 0.25;
		float num = floor(val / m);
		float s = (val - num * m) / m;
		float r, g, b;

		switch (int(num)) {
			case 0 : r = 0.0; g = s; b = 1.0; break;
			case 1 : r = 0.0; g = 1.0; b = 1.0-s; break;
			case 2 : r = s; g = 1.0; b = 0.0; break;
			case 3 : r = 1.0; g = 1.0 - s; b = 0.0; break;
		}

		return vec4(r,g,b, 1);
	}

void main(){
	vec2 coord = gl_FragCoord.xy * texelSize;
	vec4 color = texture(u_image_overlay, texCoord);
	if((color.g + color.b)/2.f <= 0.0f){ //if g and b are not in use
		if(color.r < 0){ //show negative red as positive blue
			color.b = abs(color.r);
		}
	}
	if(scene == 3){
		vec4 pressureTexel = texture(u_image_overlay, vec2(0.5f, 0.5f));
		float maxP = pressureTexel.x;
		float minP = pressureTexel.x;
		float offset = 0.08f;
		for(float i = -offset; i < 1 + offset; i += offset){
			for(float j = -offset; j < 1 + offset; j += offset){
				vec4 texel = texture(u_image_overlay, vec2(i,j));
				maxP = max(maxP, texel.x);
				minP = min(minP, texel.x);
			}
		}
		color = getSciColor(color.r, minP, maxP);
	}else if(scene == 6){
		color = vec4(color.r, color.r, color.r, color.a);
	}

	vec2 pos = gl_FragCoord.xy; //0x0 -- 700x700
	if((int(pos.x/1920.0f * 1.0f/texelSize.x) + int(pos.y/1080.0f * 1.0f/texelSize.y)) % 2 == 0){
		color += vec4(vec3(0.1f), 1.0f);
	}else{
		color += vec4(vec3(-0.1f), 1.0);
	}

	fragColor = mix(texture(u_image, texCoord), color, 0.9f);

	//fragColor = texture(u_image, texCoord);
	//fragColor = texture(u_image_overlay, vUv) * vec4(vec3(1.0f), 0.5f);
}