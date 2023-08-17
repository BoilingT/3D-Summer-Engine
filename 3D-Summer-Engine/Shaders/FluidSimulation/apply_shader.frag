#version 430
in vec2 vUv;
out vec4 fragColor;
uniform sampler2D divergenceTexture;
uniform sampler2D dyeTexture;
uniform vec3 value;

vec4 getColor(float value, float minVal, float maxVal) {
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

void main () {
	vec3 div = texture2D(divergenceTexture, vUv).rgb;
	vec4 dye = texture2D(dyeTexture, vUv);

	//vec3 result = vec3(2.5f, 0, 0) * length(clamp(div.x*value, 0.0001f, 0.0005f));

	float divValue = clamp(div.x, 0.00001f, 0.0008f); 
	vec3 result2 = value * 3.0f * divValue;
	if(dye.b > 0.0f && result2.r < 0) result2.r = 0;

	fragColor = vec4(clamp(dye.rgb + result2, 0, 1), 1.0f);
	//fragColor = vec4(dye.xyz + clamp(div.x*value, 0.00001f, 0.0005f), 1.0f);
	//fragColor = vec4(1.0f);
}