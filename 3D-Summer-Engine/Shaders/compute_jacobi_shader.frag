#version 430 core

out vec4 xNew;
uniform float alpha;	//x^2/t
uniform float rBeta;	//Reciprocal beta	1/(4+x^2/t)
uniform sampler2D x;	//x vector (Ax = b)
uniform sampler2D b;	//b vector (Ax = b)

void jacobi(vec2 coords){
	// left, right, bottom, and top x samples    
	vec4 xL = texture2D(x, coords - vec2(1, 0));
	vec4 xR = texture2D(x, coords + vec2(1, 0));
	vec4 xB = texture2D(x, coords - vec2(0, 1));   
	vec4 xT = texture2D(x, coords + vec2(0, 1)); 
	// b sample, from center     
	vec4 bC = texture2D(b, coords); 
	// evaluate Jacobi iteration   
	xNew = (xL + xR + xB + xT + alpha * bC) * rBeta;
}

void main(){
	vec2 coords = gl_FragCoord.xy;
	jacobi(coords);
	gl_FragColor = xNew;
}