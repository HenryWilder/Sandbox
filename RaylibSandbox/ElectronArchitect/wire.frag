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

	float value = 1.0 - (fragTexCoord.x);
	value *= 0.5;
	value += 0.5;

	finalColor = vec4(vec3(value), 1.0);
}