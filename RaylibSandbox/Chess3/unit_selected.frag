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

uniform vec3 selection = vec3(44, 200, 37) / 255.0;

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	vec4 texColor = texture2D(texture0, fragTexCoord);
	float alpha = 1.0 - round(texColor.a);
	vec2 space = sin(fragTexCoord * radians(180.0));
	float blend = (space.x * space.y);

	finalColor = vec4(selection, alpha * blend);
}