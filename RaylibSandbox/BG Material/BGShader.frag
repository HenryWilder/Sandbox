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

uniform float maxDepth     = 0.5;
uniform float outerDensity = 0.5;
uniform float innerDensity = 0.5;

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	// Code here

	if (distance(fragTexCoord, vec2(0.5)) <= 0.5) // Mask
	{
		finalColor = vec4(fragTexCoord * (2.0 - maxDepth), 1.0, 1.0);
	}
	else
	{
		finalColor = vec4(0.0);
	}
}
