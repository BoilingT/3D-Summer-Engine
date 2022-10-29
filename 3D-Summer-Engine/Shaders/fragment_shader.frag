#version 430 core
#define WHITE vec4(1.0f);
out vec4 FragColor;

in vec4 vColor;
in vec2 texCoord;
in float time;
in float res;

uniform sampler2D u_image;
uniform sampler2D u_image2;

void main(){
	//vec2 fragCoord = gl_FragCoord.xy;
	float x = cos(time);
	float y = sin(time);
	//vec2 offset = vec2(0, 0);
	//FragColor = texture(u_image, texCoord + offset);

	// compute 1 pixel in texture coordinates.
	vec2 pixelSize = vec2(1.0, 1.0) / res;
 
	// average the left, middle, and right pixels.
	gl_FragColor = texture(u_image, texCoord) + texture(u_image, texCoord);
}