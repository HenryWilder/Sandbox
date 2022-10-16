#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

const int numBalls = 4;
uniform vec2 p[numBalls];
uniform float r[numBalls];

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 resolution = vec2(720, 720);

void main()
{
	float activity = 0.0;
	for (int i = 0; i < numBalls; ++i)
	{
		activity += ((r[i] / resolution.x) / distance(fragTexCoord, p[i] / resolution));
	}

	finalColor = vec4((activity >= 1.0 ? mix(vec3(129,21,206)/255,vec3(29,25,40)/255, clamp(pow(activity - 1.0, 0.25),0.0,1.0)) : vec3(1.0)), 1.0);
}
