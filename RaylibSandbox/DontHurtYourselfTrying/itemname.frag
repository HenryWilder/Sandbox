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

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	// Code here

	finalColor = vec4(1.0);
}
