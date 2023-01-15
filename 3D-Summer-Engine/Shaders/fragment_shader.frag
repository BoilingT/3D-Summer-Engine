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
		if(color.r < 0){ //show negative red as positive green
			color.b = abs(color.r);
		}
	}
	if(scene == 3){
		//vec4 pressureTexel = texture(u_image_overlay, vec2(0.0f, 0.0f));
		//float maxP = pressureTexel.x;
		//float minP = pressureTexel.x;
		//for(float i = 0; i < 1; i+=0.08f){
		//	for(float j = 0; j < 1; j+=0.08f){
		//		vec4 texel = texture(u_image_overlay, vec2(i,j));
		//		maxP = max(maxP, texel.x);
		//		minP = min(minP, texel.x);
		//	}
		//}
		//color = getSciColor(color.r, minP, maxP);
	}
	fragColor = mix(texture(u_image, texCoord), color, 1.0f);
	//fragColor = texture(u_image, texCoord);
	//fragColor = texture(u_image_overlay, vUv) * vec4(vec3(1.0f), 0.5f);
}