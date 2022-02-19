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

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	// Code here

	if (distance(fragTexCoord, vec2(0.5)) <= 0.5) // Circle cutout
	{
		vec3 position = vec3(fragTexCoord * 2 - 1, 0);
		position.z = sqrt(1.0 - (position.x * position.x + position.y * position.y));

		float value = outerDensity * position.z;
		vec3 color = value < 0.5 ? mix(glow, deep, value * 2) : mix(deep, vec3(1), (value - 0.5) * 2);

		finalColor = vec4(color, 1.0 - (position.z * (1.0 - maxDepth)));
	}
	else
	{
		finalColor = vec4(0.0);
	}
}
