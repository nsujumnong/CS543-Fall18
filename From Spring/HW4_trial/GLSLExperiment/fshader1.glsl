#version 150
//#extension GL_NV_shadow_samplers_cube : enable

in  vec4  interpolatedColor;
in  vec2  texCoord;
in  vec3  R;
in  vec3  T;

in vec4 fNormal;

out vec4  fColor;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform samplerCube texMap;

uniform int usetexture;

vec4 color = vec4(0,0,0,1);

void main() 
{ 
	
	if (usetexture == 0){
		color = texture2D(texture0, texCoord);
	}
	
	else if (usetexture == 1){
		color = texture2D(texture1, texCoord);
	}

	else if (usetexture == 2){
		color = texture(texMap,R);
		//color = fNormal;
		//color = vec4(R, 1.0);
		//color = vec4(1.0, 0,0,1);
	}
	else if (usetexture == 3){
		vec4 refractColor = texture(texMap,T);
		color = mix(refractColor, vec4(1.0,1.0,1.0,1.0), 0.3);
	}
	else if (usetexture == 4){
		color = vec4(R, 1.0);
	}
	
    //fColor = interpolatedColor*color;
    //fColor = texture(texMap,R);
	fColor = color;
	//fColor = vec4(R, 1.0);
} 

