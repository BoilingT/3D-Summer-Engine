#version 430 core
#define OFFSET 0.5f

in vec2 vUv;
out vec4 fragColor;
uniform float timestep;
uniform sampler2D u;	//Input velocity
uniform sampler2D x;	//Quantity to advect
uniform vec2 texelSize;
uniform bool linearFiltering;

//Get the velocity vector from a texture at the given coordinates
vec4 f2texRect(sampler2D v, vec2 coords){
	//vec2 pixelSize = vec2(1.0f, 1.0f) / rdx;
	vec2 pos = coords / texelSize - OFFSET;
	//Texture coordinates are set from 0 to 1 in both width and height.
	return texture2D(v, (pos + OFFSET)*texelSize);
}

vec4 lerp(vec4 a, vec4 b, float k){
	return a+k*(b-a);
}

//Get a bilinearly interpolated velocity at given coordinates
vec4 f4texRECTbilerp(sampler2D q, vec2 coords){
	float offset = OFFSET;
	vec2 pos = coords / texelSize - offset;

	vec2 ic = floor(pos); //Point closest to origin
	vec2 fc = fract(pos);

	//Get four surrounding texels with quantity data and interpolate between them to find the estimated quantity
	vec4 a = texture2D(q, (ic + vec2(0, 0) + offset) * texelSize); //Bottom left
	vec4 b = texture2D(q, (ic + vec2(1, 0) + offset) * texelSize);	//Bottom right
	vec4 c = texture2D(q, (ic + vec2(1, 1) + offset) * texelSize); //Top right
	vec4 d = texture2D(q, (ic + vec2(0, 1) + offset) * texelSize);	//Top left

	//Interpolate top values -> Interpolate bottom values -> Interpolate top and bottom values
	//return a;
	//return mix(mix(a, b, fc.x), mix(d, c, fc.x), fc.y);
	return lerp(lerp(a, b, fc.x), lerp(d, c, fc.x), fc.y);
}

vec4 advect(vec2 coords){
	vec2 pos = vec2(0.0f);
	//Track the velocity back in time
	if(linearFiltering){
		pos = coords - timestep * texelSize * f4texRECTbilerp(u, coords).xy;
		return f4texRECTbilerp(x, pos);
	}else{
		pos = coords - timestep * texelSize * f2texRect(u, coords).xy;
		return f2texRect(x, pos);
	}
	//vec2 pos = coords - timestep * f2texRect(u, coords);
	//vec2 pos = coords - timestep * tx * f4texRECTbilerp(u, coords).xy;
	//Interpolate and write to the output fragment
}

//Transport quantities in the fluid
void main(){
	//Get the current coordinates relative to the window
	vec2 coords = gl_FragCoord.xy * texelSize;
	//Advect the current quantities at the current coordinates
	vec4 result = advect(coords);
	float decay = 1.0 + 0.5f * timestep;
	fragColor = result / decay;
	//gl_FragColor = vec4(f2texRect(x, coords * rdx * rdx), 0.0f, 1.0f);
	//gl_FragColor = texture2D(x, vec2(1.0f, 1.0f)/rdx * rdx * rdx * coords);
}