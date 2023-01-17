#version 430
out vec4 fragColor;
uniform vec2 texelSize;
uniform sampler2D uTexture;
uniform vec4 value;
uniform float time;

void main () {
	vec2 coords = gl_FragCoord.xy;
	vec2 margin = vec2(texelSize.x, texelSize.y) * 0;
	vec4 u = texture2D(uTexture, coords*texelSize);

	vec2 vL = (coords - vec2(1, 0));
	vec2 vR = (coords + vec2(1, 0));
	vec2 vB = (coords - vec2(0, 1));
	vec2 vT = (coords + vec2(0, 1));

	
	//if((coords.y * texelSize.y < 1.0f && coords.y * texelSize.y > 0.5f) || (coords.y * texelSize.y < 0.5f && coords.y * texelSize.y > 0.0f)){ //Floor
	//}else{
	//	fragColor = u;
	//}

	float size = 4000.0f;
	
	vec2 pos = vec2(0.5f, 0.6f) / texelSize;

	fragColor = u + value;

	if((coords.x * texelSize.x < 0.0f + margin.x || coords.x * texelSize.x > 1.0f - margin.x) || (coords.y * texelSize.y < 0.0f + margin.y || coords.y * texelSize.y > 1.0f - margin.y)){
		fragColor = u;
	}
	
	float shapeCoord = pow(coords.x - pos.x,2) + pow(coords.y - pos.y,2);
	vec2 dim = vec2(250.0f, 10.0f)/2.0f;

	bool rectCondition = (pos.x + dim.x > coords.x && pos.x - dim.x < coords.x) && (pos.y + dim.y > coords.y && pos.y - dim.y < coords.y);
	rectCondition = false;

	if((shapeCoord < size) || rectCondition){
		fragColor = vec4(0, 0, 0.0f, 0.0f);
	}

	if((shapeCoord < size + 5 && shapeCoord > size)){
		//fragColor = vec4(-u.x, -u.y, 0.0f, 1.0f);
	}
}