#version 430

out vec4 outColor;

uniform sampler2D u; //Velocities
uniform sampler2D v; //Curl
uniform vec2 texelSize;
uniform float dt;

//Use the velocities to create force
vec4 vorticityConfinement(vec2 coord, sampler2D curl){
	
	vec2 coordL = (coord - vec2(1, 0));
	vec2 coordR = (coord + vec2(1, 0));
	vec2 coordB = (coord - vec2(0, 1));
	vec2 coordT = (coord + vec2(0, 1));

	//Curl
	float cL = texture2D(curl, (coord - vec2(1, 0)) * texelSize).x;
	float cR = texture2D(curl, (coord + vec2(1, 0)) * texelSize).x;
	float cT = texture2D(curl, (coord + vec2(0, 1)) * texelSize).x;
	float cB = texture2D(curl, (coord - vec2(0, 1)) * texelSize).x;
	float c = texture2D(curl, coord * texelSize).x;

	//if (coordL.x * texelSize.x < 0.0f) { cL = -1; }
    //if (coordR.x * texelSize.x > 1.0f) { cR = -1; }
    //if (coordT.y * texelSize.y > 1.0f) { cT = -1; }
    //if (coordB.y * texelSize.y < 0.0f) { cB = -1; }

	//vec2 v = texture2D(u, coord * texelSize).xy; //Velocity
	//vec4 normalizedVorticity = normalize(curl);
	//vec3 f = cross(normalizedVorticity.xyz, curl.xyz);

	//Vorticity
	//Vorticity, ? = cross(x, c);

	//Compute Vorticity: omega = cross(x, v);

	//n = nabla |omega|; //Vorticity
	//phi = n / |n|; //Normalized vorticity
	//phi = (del |omega|)/(|del |omega|);

	//Compute force
	//force = epsilon(phi x vorticity)
	//float force = epsilon(cross(phi, omega)) * dt;
	vec2 curling = vec2(abs(cT) - abs(cB), abs(cR) - abs(cL)); //Curl vectors
	vec2 force = 0.5f * curling;
    force /= length(force) + 0.001f; //Normalize force
    force *= 30.0f * c;
    force.y *= -1.0f;
    vec2 velocity = texture2D(u, coord * texelSize).xy;
    velocity += force * dt;
    //velocity = min(max(velocity, -1000.0f), 1000.0f);

	return vec4(velocity, 0.0f, 1.0f);
}

void main(){
	vec2 coords = gl_FragCoord.xy;

	vec4 result = vorticityConfinement(coords, v);
	//vec4 result = texture2D(u, coords * texelSize);
	outColor = vec4(result.xy, 0.0f, 1.0f);
}