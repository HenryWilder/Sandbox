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
uniform vec2 cursor = vec2(0);
uniform float gridSpacing = 12;
uniform float gridRadius = 8 * 12;

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	vec2 fragPixCoord = fragTexCoord * resolution;

	// Code here

	vec2 grid = max(1 - mod(fragTexCoord * resolution, gridSpacing), 0);
	float gridLine = min(grid.x + grid.y, 1);
	float gridPoint = grid.x * grid.y;
	float mouseDistance = 1 - clamp(distance(cursor, fragPixCoord) / gridRadius, 0, 1);
	vec2 crosshare = vec2((floor(cursor.x) == floor(fragPixCoord.x) ? 1 : 0), (floor(cursor.y) == floor(fragPixCoord.y) ? 1 : 0)); 
	vec2 glow = (cos(radians(fragPixCoord * gridSpacing * 2.5)) + 1) * 0.5;

	finalColor = vec4(vec3(0.025 * (glow.x + glow.y) * mouseDistance * 2 + 0.2 * (crosshare.x + crosshare.y) + gridLine * 0.1 + gridPoint * mouseDistance * 4), 1.0);
}
