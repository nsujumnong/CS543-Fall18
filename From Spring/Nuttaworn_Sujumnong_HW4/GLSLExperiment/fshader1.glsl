#version 150

in  vec4  interpolatedColor;
in  vec3  Position;
in  vec3  Normal;
in  vec2  texCoords;
in  vec3  T;

uniform sampler2D Tex1;
uniform samplerCube RefMap;

out vec4  fColor;

void main() 
{ 
    //vec4 refractColor = textureCube(RefMap,T); // look up texture map using T
	vec4 texColor = texture(Tex1, texCoords);
	//refractcolor = mix(refractColor, WHITE, 0.3); // mix pure color with 3.0 white
	fColor = interpolatedColor*texture2D(Tex1,texCoords);
} 

