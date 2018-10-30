#version 150

in  vec4  interpolatedColor;
in  vec2  texCoord;

out vec4  fColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform int usetexture = 0;

vec4 color;

void main() 
{ 
	
	if (usetexture == 0){
		color = texture2D(texture0, texCoord);
	}
	else if (usetexture == 1){
		color = texture2D(texture1, texCoord);
	}
    fColor = interpolatedColor*color;
} 

