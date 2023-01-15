#version 430
out vec4 fragColor;
uniform vec2 texelSize;
uniform sampler2D uTexture;
uniform vec4 value;

void main () {
	vec2 coords = gl_FragCoord.xy;

	vec4 u = texture2D(uTexture, coords*texelSize);
	vec2 vL = (coords - vec2(1, 0));
	vec2 vR = (coords + vec2(1, 0));
	vec2 vB = (coords - vec2(0, 1));
	vec2 vT = (coords + vec2(0, 1));

	if(vB.y * texelSize.y < 0.0f)
	{
		fragColor.y *= -u.y;
	}
	else
	{
		fragColor = u + value;
	}

}