#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

void main(void)
{
	float time = fragTexCoord.x;

	float amp = sin(radians(time));

	finalColor = vec4(amp, vec2(0.0), 1.0);
}