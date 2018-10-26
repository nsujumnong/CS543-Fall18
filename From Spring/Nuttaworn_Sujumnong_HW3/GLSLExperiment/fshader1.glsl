#version 150

in  vec4  interpolatedColor;
//in  vec4  color;
out vec4  fColor;

void main() 
{ 
    fColor = interpolatedColor;
	//gl_FragColor = color;
} 

