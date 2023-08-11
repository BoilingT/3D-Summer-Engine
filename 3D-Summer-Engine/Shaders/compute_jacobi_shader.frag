#version 430 core
in vec2 vUv;
out vec4 fragColor;
vec4 xNew;	//out
uniform float alpha;	//x^2/t
uniform float rBeta;	//Reciprocal beta	1/(4+x^2/t)
uniform sampler2D x;	//x vector (Ax = b)
uniform sampler2D b;	//b vector (Ax = b)
uniform vec2 texelSize;

vec4 jacobi(vec2 coords){
	vec2 posL = coords - vec2(1, 0) * texelSize;
	vec2 posR = coords + vec2(1, 0) * texelSize;
	vec2 posB = coords - vec2(0, 1) * texelSize;
	vec2 posT = coords + vec2(0, 1) * texelSize;

	// left, right, bottom, and top x samples
	vec4 xL = texture2D(x, posL);
	vec4 xR = texture2D(x, posR);
	vec4 xB = texture2D(x, posB);   
	vec4 xT = texture2D(x, posT); 
	// b sample, from center     
	vec4 bC = texture2D(b, coords); 
	// evaluate one Jacobi iteration at current grid cell
	//return bC;
	//if(posL.x * texelSize.x < 0.0f){xL.x *= 0;}
	//if(posR.x * texelSize.x > 1.0f){xR.x *= 0;}
	//if(posT.y * texelSize.y > 1.0f){xT.y *= 0;}
	//if(posB.y * texelSize.y < 0.0f){xB.y *= 0;}
	return (xL + xR + xB + xT + alpha * bC) * rBeta;
	//(l + r + b + t + divergence * -1) * 0.25
}

void main(){
	vec2 coords = vUv;
	vec4 xNew = jacobi(coords);
	fragColor = vec4(xNew.xyz, 1.0f);
	//fragColor = vec4(xNew.x, 0, 0, 1.0f);
	//gl_FragColor = vec4(1.0f);
}