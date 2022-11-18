#version 430 core

vec4 xNew;	//out
uniform float alpha;	//x^2/t
uniform float rBeta;	//Reciprocal beta	1/(4+x^2/t)
uniform sampler2D x;	//x vector (Ax = b)
uniform sampler2D b;	//b vector (Ax = b)
uniform vec2 texelSize;

vec4 jacobi(vec2 coords){
	// left, right, bottom, and top x samples    
	vec4 xL = texture2D(x, (coords - vec2(1, 0)) * texelSize);
	vec4 xR = texture2D(x, (coords + vec2(1, 0)) * texelSize);
	vec4 xB = texture2D(x, (coords - vec2(0, 1)) * texelSize);   
	vec4 xT = texture2D(x, (coords + vec2(0, 1)) * texelSize); 
	// b sample, from center     
	vec4 bC = texture2D(b, coords * texelSize); 
	// evaluate one Jacobi iteration at current grid cell
	//return bC;
	return (xL + xR + xB + xT + alpha * bC) * rBeta;
}

void main(){
	vec2 coords = gl_FragCoord.xy;
	vec4 xNew = jacobi(coords);
	gl_FragColor = xNew;
	//gl_FragColor = vec4(1.0f);
}