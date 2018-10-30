#version 150

in  vec2 texCoord;
out vec4 fColor;

const float PI = 3.14159265;
uniform sampler2D texture;
uniform sampler2D textureBefore;
uniform sampler2D textureAfter;

uniform int usetexture;
uniform float uT;
uniform float uD, uR;
uniform float uMagTol;
uniform float uQuantize;

void main() 
{ 
	
	//Get image resolution
	ivec2 ires = textureSize(texture, 0);
	float ResS = float(ires.s);
	float ResT = float(ires.t);

	// Define the luminence coefficients
	vec3 W = vec3(0.299, 0.587, 0.114);
	// Get the RGB value from the image
	vec3 irgb = texture2D(texture,texCoord).rgb;
	//vec3 brgb = texture2D(textureBefore, texCoord).rgb;
	//vec3 argb = texture2D(textureAfter, texCoord).rgb;
	vec3 rgb = texture2D(texture, texCoord).rgb;
	// get the negative color vector
	vec3 negative = vec3(1.0,1.0,1.0) - irgb;
	// get the luminence value
	float luminance = dot(irgb, W);

	//Sobel Filter algorithm 
	vec2 stp0 = vec2(1.0/ResS, 0.0);
	vec2 st0p = vec2(0.0, 1.0/ResT);
	vec2 stpp = vec2(1.0/ResS, 1.0/ResT);
	vec2 stpm = vec2(1.0/ResS, -1.0/ResT);
	float i00 =   dot(texture2D(texture,texCoord).rgb        , W);
	float im1m1 = dot(texture2D(texture,texCoord - stpp).rgb , W);
	float ip1p1 = dot(texture2D(texture,texCoord + stpp).rgb , W);
	float im1p1 = dot(texture2D(texture,texCoord - stpm).rgb , W);
	float ip1m1 = dot(texture2D(texture,texCoord + stpm).rgb , W);
	float im10 =  dot(texture2D(texture,texCoord - stp0).rgb , W);
	float ip10 =  dot(texture2D(texture,texCoord + stp0).rgb , W);
	float i0m1 =  dot(texture2D(texture,texCoord - st0p).rgb , W);
	float i0p1 =  dot(texture2D(texture,texCoord + st0p).rgb , W);
	
	float h = -1.0*im1p1 - 2.0*i0p1 - 1.0*ip1p1 + 1.0*im1m1 + 2.0*i0m1 + 1.0*ip1m1;
	float v = -1.0*im1m1 - 2.0*im10 - 1.0*im1p1 + 1.0*ip1m1 + 2.0*ip10 + 1.0*ip1p1;

	float magEdge = sqrt(h*h + v*v);
	vec3 target = vec3(magEdge,magEdge,magEdge);

	//Toon shader
	float magToon = length(vec2(h,v)); 
	
	//Embossing algorithm
	vec3 c00 = texture2D(texture,texCoord).rgb;
	vec3 cp1p1 = texture2D(texture,texCoord + stpp).rgb;
	vec3 diffs = c00 - cp1p1; // vector difference
	float max = diffs.r;
	if (abs(diffs.g) > abs(max)) max = diffs.g;
	if (abs(diffs.b) > abs(max)) max = diffs.b;
	
	float gray = clamp(max + 0.5, 0.0, 1.0);
	vec3 gColor = vec3(gray, gray, gray);

	// Twirl Effect
	float Res = float(ires.s); // Just get another s value for resolution for this part
	vec2 st = texCoord;
	float radius = Res * uR;
	vec2 xy = Res * st;

	vec2 dxy = xy - Res/2;
	float r = length(dxy);
	float beta = atan(dxy.y, dxy.x) + radians(45.0) * (radius - r)/radius;
	vec2 xy1 = xy;
	if (r <= radius) {
		xy1 = Res/2. + r * vec2(cos(beta), sin(beta));
	}
	st = xy1/Res; // restore coordinates
	vec3 twirlRgb = texture2D(texture, st).rgb;

	// processed image output 
	//fColor = texture2D( texture, texCoord );				// Original Image + color
	//fColor = vec4(mix(irgb,target,uT),1.0);				// Edge detection
	fColor = vec4(luminance, luminance, luminance, 1.0);	// gray scale
	//fColor = vec4(gColor, 1.0);							// embossing effect
	//fColor = vec4(twirlRgb, 1.);							// twirling effect
	//fColor = vec4(negative, 1.0);							// negative color

	//if (magToon > uMagTol){
	//	fColor = vec4(0.0,0.0,0.0,1.0);
	//}
	//else {
	//	rgb.rgb *= uQuantize;
	//	rgb.rgb += vec3(0.5,0.5,0.5);	// round
	//	ivec3 intrgb = ivec3(rgb.rgb);	// truncate
	//	rgb.rgb = vec3(intrgb)/uQuantize;
	//	fColor = vec4(rgb, 1.);
	//}
}