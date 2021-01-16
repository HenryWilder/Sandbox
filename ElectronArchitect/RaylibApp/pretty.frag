#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 resolution = vec2(1280, 720);

void main()
{
float x = 1.0/resolution.x;
float y = 1.0/resolution.y;

vec2 pixelColor = texture2D(texture0, fragTexCoord).rg;

float mask = pixelColor.g;
float powered = pixelColor.r;

vec3 boardGrad = mix(vec3(0.0, 0.5, 0.0), vec3(0.1, 1.0, 0.2), fragTexCoord.y);
vec3 wireGrad = mix(vec3(1.0, 0.75, 0.0), vec3(1.0), (fragTexCoord.x * fragTexCoord.y));
wireGrad = mix(wireGrad, vec3(1.0, 0.0, 0.0), powered);

vec3 color = mix(boardGrad, wireGrad, mask);

finalColor = vec4(color, 1.0);
}
