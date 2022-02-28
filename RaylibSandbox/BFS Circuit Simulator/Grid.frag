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

//float Sigmoid(float x, float hardness)
//{
//	const float e = 2.71828;
//	return 1.0 / (1.0 + pow(e,-(x)));
//}
//float SCurve(float x)
//{
//	return (cos(radians(0.5 * (clamp(x,0,1) + 1))) + 1) * 0.5;
//}

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	vec2 fragPixCoord = fragTexCoord * resolution;

	// Code here

	float xGrid = clamp(1 - mod(fragTexCoord.x * resolution.x, gridSpacing), 0, 1);
	float yGrid = clamp(1 - mod(fragTexCoord.y * resolution.y, gridSpacing), 0, 1);
	float gridLine = xGrid + yGrid;
	float gridPoint = xGrid * yGrid;
	float mouseDistance = 1 - clamp(distance(cursor, fragPixCoord) / gridRadius, 0, 1);
	float crosshare = (floor(cursor.x) == floor(fragPixCoord.x) ? 1 : 0) + (floor(cursor.y) == floor(fragPixCoord.y) ? 1 : 0); 
	finalColor = vec4(vec3(gridLine * 0.1 + crosshare * 0.2 + gridPoint * mouseDistance * 4), 1.0);
}
