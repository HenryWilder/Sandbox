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

vec4 pixelData = texture2D(texture0, fragTexCoord);

vec3 temperature = mix(vec3(1.0, 0.0, 0.0),vec3(0.0, 0.0, 1.0), pixelData.g);

finalColor = vec4((temperature * vec3(pixelData.r) + (((pixelData.b * 8.0) > 3.0) ? (pixelData.b - 0.375) : 0.0)), 1.0);
}
