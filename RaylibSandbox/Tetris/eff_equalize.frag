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
	float t = fragTexCoord.x;
	float base = texture2D(texture0, vec2(t,0)).x;

	finalColor = vec4(sin(t) * base, vec2(0.0), 1.0);
}