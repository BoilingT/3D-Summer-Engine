#version 430 core

out vec4 fragColor;
vec4 xNew;	//out
uniform float alpha;	//x^2/t
uniform float rBeta;	//Reciprocal beta	1/(4+x^2/t)
uniform sampler2D x;	//x vector (Ax = b)
uniform sampler2D b;	//b vector (Ax = b)
uniform vec2 texelSize;

vec4 jacobi(vec2 coords){
	vec2 posL = coords - vec2(1, 0);
	vec2 posR = coords + vec2(1, 0);
	vec2 posB = coords - vec2(0, 1);
	vec2 posT = coords + vec2(0, 1);

	// left, right, bottom, and top x samples
	vec4 xL = texture2D(x, posL * texelSize);
	vec4 xR = texture2D(x, posR * texelSize);
	vec4 xB = texture2D(x, posB * texelSize);   
	vec4 xT = texture2D(x, posT * texelSize); 
	// b sample, from center     
	vec4 bC = texture2D(b, coords * texelSize); 
	// evaluate one Jacobi iteration at current grid cell
	//return bC;
	//if(posL.x * texelSize.x < 0.0f){xL.x *= 0;}
	//if(posR.x * texelSize.x > 1.0f){xR.x *= 0;}
	//if(posT.y * texelSize.y > 1.0f){xT.y *= 0;}
	//if(posB.y * texelSize.y < 0.0f){xB.y *= 0;}
	return (xL + xR + xB + xT + alpha * bC) * rBeta;
}

void main(){
	vec2 coords = gl_FragCoord.xy;
	vec4 xNew = jacobi(coords);
	fragColor = vec4(xNew.x, 0, 0, 1.0f);
	//gl_FragColor = vec4(1.0f);
}