#version 330 core
out vec4 FragColor;

in vec3 vertexPos;
//in vec3 colorValues;
in vec2 textureCoord;

uniform vec4 ourColor;
uniform vec4 color;
uniform sampler2D texture1;
uniform sampler2D texture2;

void main(){
	vec3 colorValues = vec3(1,0,0);
	//FragColor = vec4(colorValues, 1.0f) / vec4(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y, (sin(-vertexPos.y)*30-sin(vertexPos.x*25))*ourColor.y*0.5f , 0.0f, 1.0f);
	//FragColor = vec4(colorValues, 1.0f) / vec4(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y, 1.0f , 0.0f, 1.0f);
	//FragColor = vec4(colorValues, 1.0f) / (vec4(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y, sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y , sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y, 1.0f));
	//FragColor = vec4(colorValues, 1.0f) / (vec4(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y + ourColor.y*10, sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y + ourColor.y*50 , sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y + ourColor.y*50, 1.0f));
	//FragColor = vec4(colorValues, 1.0f) / (vec4(abs(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25 + ourColor.z)*20* ourColor.y + ourColor.y*10), abs(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y + ourColor.y*50),abs(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y + ourColor.y*50), 1.0f));
	//FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	vec4 effectColor = vec4(colorValues, 1.0f) / (vec4(abs(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25 + ourColor.z)*20* ourColor.y + ourColor.y*10), abs(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y + ourColor.y*50),abs(sin(-vertexPos.y*20)*20-sin(vertexPos.x*25)*20* ourColor.y + ourColor.y*50), 1.0f));
	FragColor = effectColor;
	//FragColor = effectColor / texture(texture1, textureCoord);
	//FragColor = mix(texture(texture1, textureCoord), texture(texture2, textureCoord), 0.2f);
	//FragColor = color;
}