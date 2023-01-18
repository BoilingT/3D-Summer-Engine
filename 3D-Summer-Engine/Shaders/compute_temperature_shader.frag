#version 430
out vec4 fragColor;
uniform vec2 texelSize;
uniform sampler2D u; //Velocity buffer
uniform sampler2D t; //Temperature buffer
uniform sampler2D d; //Density buffer
uniform float T0;    //Ambient temperature
uniform float s;    //Temperature scalar
uniform float k;     //Constant mass factor
uniform float dt;

//Calculate and apply boyancy force
void main () {
    
    vec2 coords = gl_FragCoord.xy;
    float temp = texture(t, coords * texelSize).x; 
    float density = texture(d, coords * texelSize).x; 
    vec2 j_hat = vec2(0.0f, -1.0f); //Vertical direction
    vec2 bouyancy_force = (-k * density + s * (temp - T0)) * j_hat;
    
    vec2 velocity = texture(u, coords * texelSize).xy;
    velocity += bouyancy_force * dt;

    fragColor = vec4(velocity, 0.0f, 1.0f);
} 