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
uniform float tauX, tauY, aX, aY;
uniform float rho;


void main() 
{ 
	
	//Get image resolution
	ivec2 ires = textureSize(texture, 0);
	float ResS = float(ires.s);
	float ResT = float(ires.t);

	// Define the luminence coefficients
	vec3 W = vec3(0.2125, 0.7154, 0.0721);
	// Get the RGB value from the image
	vec3 irgb = texture2D(texture,texCoord).rgb;
	vec3 rgb = texture2D(texture, texCoord).rgb;
	// get the negative color vector
	vec3 negative = vec3(1.0,1.0,1.0) - irgb;
	// get the luminence value
	float luminance = dot(irgb, W);

	//-----------------------------------------------------------------------------
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
	
	// apply the H and V Sobel filter at the pixel
	float h = -1.0*im1p1 - 2.0*i0p1 - 1.0*ip1p1 + 1.0*im1m1 + 2.0*i0m1 + 1.0*ip1m1;
	float v = -1.0*im1m1 - 2.0*im10 - 1.0*im1p1 + 1.0*ip1m1 + 2.0*ip10 + 1.0*ip1p1;

	float magEdge = sqrt(h*h + v*v);
	vec3 target = vec3(magEdge,magEdge,magEdge);

	//-----------------------------------------------------------------------------
	//Toon shader
	float magToon = length(vec2(h,v)); 
	
	//-----------------------------------------------------------------------------
	//Embossing algorithm
	vec3 c00 = texture2D(texture,texCoord).rgb;
	vec3 cp1p1 = texture2D(texture,texCoord + stpp).rgb;
	vec3 diffs = c00 - cp1p1; // vector difference
	float max = diffs.r;
	if (abs(diffs.g) > abs(max)) max = diffs.g;
	if (abs(diffs.b) > abs(max)) max = diffs.b;
	
	float gray = clamp(max + 0.5, 0.0, 1.0);
	vec3 gColor = vec3(gray, gray, gray);

	//-----------------------------------------------------------------------------
	// Twirl Effect
	float Res = float(ires.s); 
	vec2 st = texCoord;
	float radius = Res * uR;
	vec2 xy = Res * st;

	vec2 dxy = xy - Res/2;
	float r = length(dxy);
	float beta = atan(dxy.y, dxy.x) + radians(uD) * (radius - r)/radius;
	vec2 xy1 = xy;
	if (r <= radius) {
		xy1 = Res/2. + r * vec2(cos(beta), sin(beta));
	}
	st = xy1/Res; // restore coordinates
	vec3 twirlRgb = texture2D(texture, st).rgb;

	//-----------------------------------------------------------------------------
	// ripple effect
	float x2 = xy.x + aX*sin((2*PI*xy.y)/tauX);
	float y2 = xy.y + aY*sin((2*PI*xy.x)/tauY);
	vec2 xy2 = vec2(x2,y2);
	vec2 st2 = xy2/Res; // restore coordinates
	vec3 rippleRgb = texture2D(texture, st2).rgb;

	//-----------------------------------------------------------------------------
	// spherical effect
	float sphRes = float(ires.s);
	vec2 dxy3 = xy - Res/2;
	float r_max = sphRes * uR;
	float r3 = length(dxy3);
	float z = sqrt(r_max*r_max - r3*r3);
	float betaX = (1-(1/rho))*asin(dxy3.x/sqrt(dxy3.x*dxy3.x + z*z));
	float betaY = (1-(1/rho))*asin(dxy3.y/sqrt(dxy3.y*dxy3.y + z*z));
	vec2 xy3 = xy;
	if (r3 <= r_max){
		float x3 = xy.x - z*tan(betaX);
		float y3 = xy.y - z*tan(betaY);
		xy3 = vec2(x3,y3);
	}
	vec2 st3 = xy3/sphRes; // restore coordinates
	vec3 sphereRgb = texture2D(texture, st3).rgb;

	//-----------------------------------------------------------------------------
	// processed image output 
	if (usetexture == 0){
		fColor = texture2D( texture, texCoord );				// Original Image + color
	}
	else if (usetexture == 1){
		fColor = vec4(luminance, luminance, luminance, 1.0);	// gray scale
	}
	else if (usetexture == 2){
		fColor = vec4(gColor, 1.0);								// embossing effect
	}
	else if (usetexture == 3){
		fColor = vec4(negative, 1.0);							// negative color
	}
	else if (usetexture == 4){
		fColor = vec4(mix(irgb,target,uT),1.0);					// Edge detection
	}
	else if (usetexture == 5){
		fColor = vec4(twirlRgb, 1.);							// twirling effect
	}
	// toon shading effect
	else if (usetexture == 6){
		if (magToon > uMagTol){
			fColor = vec4(0.0,0.0,0.0,1.0);
		}
		else {
			rgb.rgb *= uQuantize;
			rgb.rgb += vec3(0.5,0.5,0.5);	// round
			ivec3 intrgb = ivec3(rgb.rgb);	// truncate
			rgb.rgb = vec3(intrgb)/uQuantize;
			fColor = vec4(rgb, 1.0);
		}
	}
	else if (usetexture == 7){
		fColor = vec4(rippleRgb, 1.0);							// Ripple Effect
	}
	else if (usetexture == 8){
		fColor = vec4(sphereRgb, 1.0);							// Spherical Effect
	}
	
}