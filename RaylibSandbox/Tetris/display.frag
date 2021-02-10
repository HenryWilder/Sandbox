#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

uniform vec2 resolution = vec2(1280, 720);

void main(void)
{
	float x = 1 / resolution.x;
	float y = 1 / resolution.y;

	float fragInputVal = texture2D(texture0, vec2(fragTexCoord.x, 0.0)).x;

	float dist = (fragInputVal + 0.5 - fragTexCoord.y);

	finalColor = vec4(fragTexCoord.y, vec2(0.0), 1.0);
}