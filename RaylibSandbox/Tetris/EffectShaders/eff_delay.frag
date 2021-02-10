#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform float totalSize = 1024;

// Output fragment color
out vec4 finalColor;

void main(void)
{
	float time = fragTexCoord.x;
	float t = 1 / totalSize;
	float source = texture2D(texture0, vec2(t,0)).x;

	float amp = 0.0;

	for (float i = 0.0; i < time; i += t)
	{
		amp += (texture2D(texture0, vec2(i,0)).x * ((1.0 - (i - time)) * t));
	}

	finalColor = vec4(amp, vec2(0.0), 1.0);
}