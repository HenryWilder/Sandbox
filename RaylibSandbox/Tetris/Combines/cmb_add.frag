#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform sampler2D texture1;
//uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

void main()
{
	float a = texture2D(texture0, fragTexCoord).x;
	float b = texture2D(texture1, fragTexCoord).x;

	finalColor = vec4(a + b, vec2(0.0), 1.0);
}