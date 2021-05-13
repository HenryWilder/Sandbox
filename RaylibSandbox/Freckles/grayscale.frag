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

void main()
{
	vec4 color = texture(texture0, fragTexCoord);
	float gray = (color.r + color.g + color.b) / 3.0;
	finalColor = vec4(vec3(gray),1.0);
}
