#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

// Input uniform heights
uniform sampler2D texture0;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 resolution = vec2(1280, 720);

//uniform vec2 cameraPosition = vec2(0.0, 0.0);
//uniform float cameraZoom = float(1.0);

void main()
{
float x = 1.0/resolution.x;
float y = 1.0/resolution.y;

vec4 pixelData = texture2D(texture0, fragTexCoord);

float gridline = float(0.0);
if (int(fragTexCoord.x * resolution.x) % 16 == 0) gridline = 1.0;

finalColor = vec4(gridline,0.0,0.0,1.0);
}
