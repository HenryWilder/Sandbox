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

uniform vec3 fill		= vec3(80, 80, 90) / 255.0;
uniform vec3 shade		= vec3(50, 55, 60) / 255.0;
uniform vec3 outline	= vec3(40, 30, 50) / 255.0;
uniform vec3 shine		= vec3(100, 100, 110) / 255.0;

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	vec4 texColor = texture2D(texture0, fragTexCoord);
	vec3 color = vec3(0.0);
	color = mix(color, fill,	texColor.r);
	color = mix(color, shade,	texColor.g);
	color = mix(color, outline,	texColor.b);
	color = mix(color, shine,	texColor.r * texColor.g * texColor.b);

	finalColor = vec4(color.rgb, round(texColor.a));
}