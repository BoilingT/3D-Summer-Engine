#version 430 core
#define WHITE vec4(1.0f);

out vec4 fragColor;
in vec4 vColor;
in vec2 vUv;
in vec2 texCoord;

uniform sampler2D u_image;
uniform sampler2D u_image_overlay;
uniform vec2 texelSize;

void main(){
	vec2 coord = gl_FragCoord.xy * texelSize;
	vec4 color = texture(u_image_overlay, vUv);
	if((color.g + color.b)/2.f <= 0){ //if g and b are not in use
		if(color.r < 0){ //show negative red as positive green
			color.b = abs(color.r);
		}
	}
	//fragColor = mix(texture(u_image, vUv + vec2(0.5f, 0.0f)), color, 0.7f);
	fragColor = texture(u_image, vUv + vec2(0.5f, 0.5f));
	//fragColor = texture(u_image_overlay, vUv) * vec4(vec3(1.0f), 0.5f);
}