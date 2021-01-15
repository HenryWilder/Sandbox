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

float rand(vec2 coord);
float rand(vec2 coord){
    return fract(sin(dot(coord.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main()
{
float x = 1.0/resolution.x;
float y = 1.0/resolution.y;

float value = round(rand(fragTexCoord) - 0.25);
finalColor = vec4(value, 0.0, 0.0, 1.0);
}
