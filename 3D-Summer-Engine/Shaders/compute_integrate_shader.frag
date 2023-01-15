#version 430
out vec4 fragColor;
uniform vec2 texelSize;
uniform sampler2D uTexture;
uniform vec4 value;

void main () {
	vec2 coords = gl_FragCoord.xy;
	vec2 margin = vec2(texelSize.x*100, texelSize.y)*0;
	vec4 u = texture2D(uTexture, coords*texelSize);
	fragColor = u + value;

	vec2 vL = (coords - vec2(1, 0));
	vec2 vR = (coords + vec2(1, 0));
	vec2 vB = (coords - vec2(0, 1));
	vec2 vT = (coords + vec2(0, 1));

	if((coords.x * texelSize.x < 0.0f + margin.x || coords.x * texelSize.x > 1.0f - margin.x) || (coords.y * texelSize.y < 0.0f + margin.y || coords.y * texelSize.y > 1.0f - margin.y)){
		fragColor = u;
	}
}