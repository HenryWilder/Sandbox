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
uniform vec3 black = vec3(0.925490196, 0.654901961, 0.37254902);
uniform vec3 white = vec3(0.988235294, 0.858823529, 0.650980392);

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	vec2 space = sin(fragTexCoord * radians(360.0) * 4.0);
	float blend = ((space.x * space.y) + 1.0) * 0.5;

	finalColor = vec4(mix(black, white, blend), 1.0);
}