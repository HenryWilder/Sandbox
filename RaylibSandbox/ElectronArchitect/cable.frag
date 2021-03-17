#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// Custom uniforms
uniform vec2 resolution = vec2(1280, 720);
uniform vec2 cursorPos = vec2(0.0);

void main(void)
{
	float x = 1 / resolution.x;
	float y = 1 / resolution.y;

	finalColor = vec4(fragTexCoord.x, fragTexCoord.y, 0.0, 1.0);
}