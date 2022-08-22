#version 330 core
out vec4 FragColor;

in vec3 vertexPos;

uniform vec4 ourColor;

void main(){
	FragColor = vec4(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y, (sin(-vertexPos.y)*30-sin(vertexPos.x*25))*ourColor.y*0.5f , 0.0f, 1.0f);
}