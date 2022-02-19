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

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	// Code here

	if (distance(fragTexCoord, vec2(0.5)) <= 0.5) // Circle cutout
	{
		vec3 position = vec3(fragTexCoord * 2 - 1, 0);
		position.z = sqrt(1.0 - (position.x * position.x + position.y * position.y));

		float dist = distance(position, lightSource);
		float lightStrength = (maxDepth * 0.25) / (dist * dist);

		float sliceVolume = position.z * maxDepth; // Assume width/height of 1
		float averageDensity = (outerDensity * 2 + innerDensity) / 3;
		float sliceMass = averageDensity * sliceVolume;

		// Cell
		vec3 color = mix(mix(mix(glow, refl, lightStrength), deep, outerDensity), mix(deep, mix(glow * 20, refl * 20, innerDensity), innerDensity), innerDensity);
		vec3 shallowColor = mix(deep, vec3(glow * 20), clamp((outerDensity - 0.75) * 3,0,1));
		vec3 deepColor	  = mix(glow, vec3(refl * 20), clamp((innerDensity - 0.75) * 3,0,1));
		finalColor = vec4((lightStrength < 1.0 ? shallowColor : refl), 1.0);
		// Soft
		//finalColor = vec4(vec3(mix(deep, glow, 1-sliceMass)), 1.0);
	}
	else
	{
		finalColor = vec4(0.0);
	}
}
