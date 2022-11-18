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
	fragColor = texture(u_image, vUv) + texture(u_image_overlay, vUv) * vec4(vec3(1.0f), 0.5f);
	//fragColor = texture(u_image_overlay, vUv) * vec4(vec3(1.0f), 0.5f);
}