#version 150

in  vec4  interpolatedColor;
in  vec2  texCoord;

out vec4  fColor;

uniform sampler2D texture;

void main() 
{ 
    fColor = interpolatedColor*texture2D(texture, texCoord);
} 

