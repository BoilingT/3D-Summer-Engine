#version 430 core
#define WHITE vec4(1.0f);

out vec4 fragColor;
in vec4 vColor;
in vec2 vUv;
in vec2 texCoord;

uniform sampler2D u_image;
uniform sampler2D u_image_overlay;
uniform vec2 dyeTexelSize;
uniform vec2 velTexelSize;
uniform vec2 viewportBufferSize;
uniform int scene;
uniform float t;

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

float gaussian(float x, float a, float b, float c){
	return a*exp(-pow(x-b,2)/(2*c*c));
}

void main(){
	vec4 color = texture(u_image_overlay, texCoord);
	if(scene == 1 && false){
		float vel = length(color.rg);
		if(vel == 0){
			color = vec4(1, 1, 1, 1.0f) * gaussian(vel, 1, 0, 100);
		}/*else if(color.r > 0 && color.g < 0){
			color.r = gaussian(vel, 1, 0, 100);
		}else if(color.r < 0){
			color.b = gaussian(vel, 1, 0, 100);
		}*/
		else if(vel > 0){
			color = vec4(vec3(gaussian(color.r, 1, 0, 100), 1, 1), 1.0f);
		}

		/*if(color.g > 0){
			color.r = gaussian(vel, 1, 0, 100);
		}else if(color.g < 0){
			color.b = gaussian(vel, 1, 0, 100);
		}*/

	}

	if((color.g + color.b)/2.f <= 0.0f){ //if g and b are not in use
		if(color.r < 0){ //show negative red as positive blue
			color.b = abs(color.r);
		}
	}

	if(scene == 3){
		vec2 pressureTexel = texture(u_image_overlay, vec2(0.0f, 0.0f)).xy;
		float maxP = pressureTexel.x;
		float minP = pressureTexel.x;
		float offset = 0.08f;
		for(float i = 0; i < 1; i += offset){
			for(float j = 0; j < 1; j += offset){
				vec2 texel = texture(u_image_overlay, vec2(i,j)).xy;
				maxP = max(maxP, texel.x);
				minP = min(minP, texel.x);
			}
		}
		/*for(float i = 0; i < 1; i += offset.x){
			vec2 texel = texture(u_image_overlay, vec2(i,i)).xy;
			maxP = max(maxP, texel.x);
			minP = min(minP, texel.x);
		}*/
		color = getSciColor(color.r, minP, maxP);
	}else if(scene == 6){
		color = vec4(color.r, color.r, color.r, color.a);
	}

	/*vec2 screen = viewportBufferSize;
	vec2 pos = gl_FragCoord.xy;
	if((int(pos.x/screen.x * 1.0f/dyeTexelSize.x) + int(pos.y/screen.y * 1.0f/dyeTexelSize.y)) % 2 == 0){
		color += vec4(vec3(0.1f), 1.0f);
	}else{
		color += vec4(vec3(0.01f), 1.0);
	}
	if((int(pos.x/screen.x * 1.0f/velTexelSize.x) + int(pos.y/screen.y * 1.0f/velTexelSize.y)) % 2 == 0){
		color += vec4(0.15f, 0.0f, 0.0f, 1.0f);
	}else{
		color += vec4(0.0f, 0.0f, 0.1f, 1.0f);
	}*/

	fragColor = mix(texture(u_image, texCoord), color, 0.9f);
	/*float ratio = 1.0f / (dyeTexelSize.x / dyeTexelSize.y);
	vec2 coords = gl_FragCoord.xy;
	vec2 pos = vec2(0.5f, 0.5f) * vec2(2560, 1440);
	const float pi = 3.14159f;
	float r = 250.f * ratio;
	float v = t;
	float xB = pos.x - (r * sin(-v)) / 2.f;
	float yB = pos.y + (r * cos(-v)) / 2.f;

	float xC = pos.x - (r * sin(-(v + 2*pi/3.f))) / 2.f;
	float yC = pos.y + (r * cos(-(v + 2*pi/3.f))) / 2.f;

	float xD = pos.x - (r * sin(-(v - 2*pi/3.f))) / 2.f;
	float yD = pos.y + (r * cos(-(v - 2*pi/3.f))) / 2.f;

	float mBC = (yC - yB) / (xC - xB);
	float mBD = (yD - yB) / (xD - xB);
	float mDC = (yC - yD) / (xC - xD);

	bool y1 = mBC*(coords.x - xB) - coords.y > -yB;
	bool y2 = mBD*(coords.x - xB) - coords.y > -yB;
	bool y3 = mDC*(coords.x - xC) - coords.y < -yC;

	if(xB >= xC) y1 = mBC*(coords.x - xB) - coords.y < -yB;
	if(xD >= xB) y2 = mBD*(coords.x - xB) - coords.y < -yB;
	if(xD >= xC) y3 = mDC*(coords.x - xC) - coords.y > -yC;

	bool triangleCondition = y1 && y2 && y3;
	float shapeCoord = pow(coords.x - pos.x,2) + pow(coords.y - pos.y,2);

	float B = pow(coords.x - xB,2) + pow(coords.y - yB,2);
	float C = pow(coords.x - xC,2) + pow(coords.y - yC,2);
	float D = pow(coords.x - xD,2) + pow(coords.y - yD,2);

	bool circleCondition = shapeCoord < 3.f;
	bool Bc = B < 3.f;
	bool Cc = C < 3.f;
	bool Dc = D < 3.f;

	if(triangleCondition){
		fragColor = vec4(1);
	}
	if(Bc){
		fragColor = vec4(1,0,0,1);
	}
	if(Cc){
		fragColor = vec4(0,1,0,1);
	}
	if(Dc){
		fragColor = vec4(0,0,1,1);
	}*/
	//fragColor = color;
	//fragColor = texture(u_image, texCoord);
	//fragColor = texture(u_image_overlay, vUv) * vec4(vec3(1.0f), 0.5f);
}