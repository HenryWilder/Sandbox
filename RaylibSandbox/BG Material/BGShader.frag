#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 resolution = vec2(1280, 720);

vec3 deep = vec3(29,25,40)   / vec3(255);
vec3 glow = vec3(129,21,206) / vec3(255);
vec3 refl = vec3(38,144,252) / vec3(255);

uniform float maxDepth     = 0.5;
uniform float outerDensity = 0.5;
uniform float innerDensity = 0.5;

uniform vec3 lightSource = vec3(0.5,-0.5,1.0);
uniform float maskSubdiv = 1.0 / 3.0;

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	// Code here

	float mask = 0;
	mask += (distance(fragTexCoord + vec2(x * -maskSubdiv, y * -maskSubdiv), vec2(0.5)) < 0.5 ? 1.0 : 0.0);
	mask += (distance(fragTexCoord + vec2(x *           0, y * -maskSubdiv), vec2(0.5)) < 0.5 ? 1.0 : 0.0);
	mask += (distance(fragTexCoord + vec2(x * +maskSubdiv, y * -maskSubdiv), vec2(0.5)) < 0.5 ? 1.0 : 0.0);
	mask += (distance(fragTexCoord + vec2(x * -maskSubdiv, y *           0), vec2(0.5)) < 0.5 ? 1.0 : 0.0);
	mask += (distance(fragTexCoord + vec2(x *           0, y *           0), vec2(0.5)) < 0.5 ? 1.0 : 0.0);
	mask += (distance(fragTexCoord + vec2(x * +maskSubdiv, y *           0), vec2(0.5)) < 0.5 ? 1.0 : 0.0);
	mask += (distance(fragTexCoord + vec2(x * -maskSubdiv, y * +maskSubdiv), vec2(0.5)) < 0.5 ? 1.0 : 0.0);
	mask += (distance(fragTexCoord + vec2(x *           0, y * +maskSubdiv), vec2(0.5)) < 0.5 ? 1.0 : 0.0);
	mask += (distance(fragTexCoord + vec2(x * +maskSubdiv, y * +maskSubdiv), vec2(0.5)) < 0.5 ? 1.0 : 0.0);
	mask /= 9.0;

	//vec3 position = vec3(fragTexCoord * 2 - 1, 0);
	//position.z = sqrt(1.0 - (position.x * position.x + position.y * position.y));
	//
	//float dist = distance(position, lightSource);
	//float lightStrength = (abs(dot(normalize(position), normalize(lightSource))) * 0.25) / (dist * maxDepth);
	//
	//float sliceVolume = position.z * maxDepth; // Assume width/height of 1
	//float averageDensity = (outerDensity * 2 + innerDensity) / 3;
	//float sliceMass = averageDensity * sliceVolume;
	//
	//vec3 base = mix(deep, glow, 1.0 - position.z);
	//vec3 shaded = mix(base, refl, lightStrength);
	finalColor = vec4(vec3(1.0), mask);
}
