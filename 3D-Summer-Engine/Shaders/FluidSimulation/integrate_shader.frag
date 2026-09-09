#version 430
in vec2 vUv;
out vec4 fragColor;
uniform vec2 texelSize;
uniform sampler2D uTexture;
uniform vec4 value;
uniform float time;
uniform float t;

void main () {
	float ratio = 1.0f / (texelSize.x / texelSize.y);
	vec2 coords = gl_FragCoord.xy;
	vec2 margin = vec2(texelSize.x*200, texelSize.y*100) * 1.f;
	vec4 u = texture2D(uTexture, vUv);
	const float pi = 3.14159f;
	//if((coords.y * texelSize.y < 1.0f && coords.y * texelSize.y > 0.5f) || (coords.y * texelSize.y < 0.5f && coords.y * texelSize.y > 0.0f)){ //Floor
	//}else{
	//	fragColor = u;
	//}

	float size = 50.f * ratio;
	
	vec2 pos = vec2(0.5f, 0.6f) / texelSize;

	fragColor = u + value;

	if((coords.x * texelSize.x < 0.0f + margin.x || coords.x * texelSize.x > 1.0f - margin.x) || (coords.y * texelSize.y < 0.0f + margin.y || coords.y * texelSize.y > 1.0f - margin.y)){
		fragColor = u;
	}
	
	float shapeCoord = pow(coords.x - pos.x,2) + pow(coords.y - pos.y,2);
	vec2 dim = vec2(250.0f, 10.0f)/2.0f;

	bool rectCondition = (pos.x + dim.x > coords.x && pos.x - dim.x < coords.x) && (pos.y + dim.y > coords.y && pos.y - dim.y < coords.y);
	rectCondition = false;
	
	float r = 1.9f*size * ratio;
	float v = pi;
	float xB = pos.x - (r * sin(-v)) / 2.f;
	float yB = pos.y + (r * cos(-v)) / 2.f;

	float xC = pos.x - (r * sin(-(v + 2*pi/3.f))) / 2.f;
	float yC = pos.y + (r * cos(-(v + 2*pi/3.f))) / 2.f;

	float xD = pos.x - (r * sin(-(v - 2*pi/3.f))) / 2.f;
	float yD = pos.y + (r * cos(-(v - 2*pi/3.f))) / 2.f;

	float mBC = (yC - yB) / (xC - xB);
	float mBD = (yD - yB) / (xD - xB);
	float mDC = (yC - yD) / (xC - xD);

	bool y1 = mBC*(coords.x - xB) - coords.y >= -yB;
	bool y2 = mBD*(coords.x - xB) - coords.y >= -yB;
	bool y3 = mDC*(coords.x - xC) - coords.y <= -yC;

	if(xB >= xC) y1 = mBC*(coords.x - xB) - coords.y <= -yB;
	if(xD >= xB) y2 = mBD*(coords.x - xB) - coords.y <= -yB;
	if(xD >= xC) y3 = mDC*(coords.x - xC) - coords.y >= -yC;
	
	//pos = vec2(0.8f, 0.6f) / texelSize;
	float r2 = r;
	v = 0;
	float xB2 = pos.x - (r2 * sin(-v)) / 2.f;
	float yB2 = pos.y + (r2 * cos(-v)) / 2.f;

	float xC2 = pos.x - (r2 * sin(-(v + 2*pi/3.f))) / 2.f;
	float yC2 = pos.y + (r2 * cos(-(v + 2*pi/3.f))) / 2.f;

	float xD2 = pos.x - (r2 * sin(-(v - 2*pi/3.f))) / 2.f;
	float yD2 = pos.y + (r2 * cos(-(v - 2*pi/3.f))) / 2.f;

	float mB2C = (yC2 - yB2) / (xC2 - xB2);
	float mB2D = (yD2 - yB2) / (xD2 - xB2);
	float mD2C = (yC2 - yD2) / (xC2 - xD2);

	bool y12 = mB2C*(coords.x - xB2) - coords.y >= -yB2;
	bool y22 = mB2D*(coords.x - xB2) - coords.y >= -yB2;
	bool y32 = mD2C*(coords.x - xC2) - coords.y <= -yC2;

	if(xB2 >= xC2) y12 = mB2C*(coords.x - xB2) - coords.y <= -yB2;
	if(xD2 >= xB2) y22 = mB2D*(coords.x - xB2) - coords.y <= -yB2;
	if(xD2 >= xC2) y32 = mD2C*(coords.x - xC2) - coords.y >= -yC2;

	bool triangleCondition = y1 && y2 && y3;
	bool triangleCondition2 = y12 && y22 && y32;
	//bool triangleCondition =   ((coords.x - pos.x - coords.y + pos.y) > -h/2.f)
	//						&& ((pos.x - coords.x - coords.y + pos.y) > -h/2.f) 
	//						&& (pos.y - coords.y) < h/2.f; 
	bool circleCondition = shapeCoord < size * size;
	//circleCondition = false;
	//triangleCondition = false;
	//triangleCondition2 = false;
	if(triangleCondition || circleCondition || triangleCondition2){
		fragColor = -u;
	}
	if(circleCondition || triangleCondition2 || triangleCondition2){
		fragColor = vec4(0);
	}
}