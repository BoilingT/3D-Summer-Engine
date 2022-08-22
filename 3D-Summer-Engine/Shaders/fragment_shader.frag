#version 330 core
out vec4 FragColor;

in vec3 vertexPos;
in vec3 colorValues;

uniform vec4 ourColor;

void main(){
	//FragColor = vec4(colorValues, 1.0f) / vec4(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y, (sin(-vertexPos.y)*30-sin(vertexPos.x*25))*ourColor.y*0.5f , 0.0f, 1.0f);
	//FragColor = vec4(colorValues, 1.0f) / vec4(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y, 1.0f , 0.0f, 1.0f);
	//FragColor = vec4(colorValues, 1.0f) / (vec4(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y, sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y , sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y, 1.0f));
	//FragColor = vec4(colorValues, 1.0f) / (vec4(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y + ourColor.y*10, sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y + ourColor.y*50 , sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y + ourColor.y*50, 1.0f));
	//FragColor = vec4(colorValues, 1.0f) / (vec4(abs(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25 + ourColor.z)*20* ourColor.y + ourColor.y*10), abs(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y + ourColor.y*50),abs(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y + ourColor.y*50), 1.0f));

	FragColor = vec4(colorValues, 1.0f);
}