#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec2 cursor = vec2(0);

uniform vec2 p0 = vec2(0);
uniform float r0 = 0;
uniform vec2 p1 = vec2(0);
uniform float r1 = 0;
uniform vec2 p2 = vec2(0);
uniform float r2 = 0;
uniform vec2 p3 = vec2(0);
uniform float r3 = 0;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 resolution = vec2(720, 720);

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	
	vec2 p[4];
	p[0] = p0;
	p[1] = p1;
	p[2] = p2;
	p[3] = p3;

	float r[4];
	r[0] = r0;
	r[1] = r1;
	r[2] = r2;
	r[3] = r3;

	float activity = 0.0;
	for (int i = 0; i < 4; ++i)
	{
		activity += ((r[i] / resolution.x) / distance(fragTexCoord, p[i] / resolution));
	}

	finalColor = vec4((activity >= 1.0 ? mix(vec3(129,21,206)/255,vec3(29,25,40)/255, clamp(pow(activity - 1.0, 0.25),0.0,1.0)) : vec3(1.0)), 1.0);
}
