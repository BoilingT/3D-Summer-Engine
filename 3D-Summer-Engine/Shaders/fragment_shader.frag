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
	if((color.g + color.b)/2.f <= 0){
		if(color.r < 0){
			color.b = abs(color.r);
		}
	}
	fragColor = mix(texture(u_image, vUv), color, 0.6f);
	//fragColor = texture(u_image_overlay, vUv) * vec4(vec3(1.0f), 0.5f);
}