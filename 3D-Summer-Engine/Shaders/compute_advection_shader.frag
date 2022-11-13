#version 430 core
vec4 xNew; //Out
uniform float timestep;
uniform float rdx;		//1 / Grid scale
uniform sampler2D u;	//Input velocity
uniform sampler2D x;	//Quantity to advect


//Get the velocity vector from a texture at the given coordinates
vec2 f2texRect(sampler2D v, vec2 coords){
	vec2 pixelSize = vec2(1.0, 1.0) / rdx;
	return texture2D(v, pixelSize*coords);
}

vec4 lerp(vec4 a, vec4 b, float k){
	return a+k*(b-a);
}

//Get a bilinearly interpolated velocity at given coordinates
vec4 f4texRECTbilerp(sampler2D q, vec2 coords){

	vec2 ic = floor(coords); //Point closest to origin
	vec2 fc = fract(coords);

	//Get four surrounding texels with quantity data and interpolate between them to find the estimated quantity
	vec4 a = texture2D(q, ic);
	vec4 b = texture2D(q, ic + vec2(1, 0));
	vec4 c = texture2D(q, ic + vec2(1, 1));
	vec4 d = texture2D(q, ic + vec2(0, 1));

	//Interpolate top values -> Interpolate bottom values -> Interpolate top and bottom values
	return lerp(lerp(a, b, fc.x), lerp(d, c, fc.x), fc.y);
}

vec4 advect(vec2 coords){
	//Track the velocity back in time
	vec2 pos = coords - timestep * rdx * f2texRect(u, coords); 
	//Interpolate and write to the output fragment
	return f4texRECTbilerp(x, pos);
}

//Transport quantities in the fluid
void main(){
	//Get the current coordinates
	vec2 coords = gl_FragCoord.xy;
	//Advect the current quantities at the current coordinates
	xNew = advect(coords);
	gl_FragColor = xNew;
}