#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 resolution = vec2(800, 450);

void main() 
{
    float x = 1.0/resolution.x;
    float y = 1.0/resolution.y;

    float radius = 50.0;
    vec2 pos = vec2(fragTexCoord.x / x, fragTexCoord.y / y);
    vec2 offsetPos = pos - (resolution / 2);
    float height = sqrt((radius * radius) - ((offsetPos.x * offsetPos.x) + (offsetPos.y * offsetPos.y))) / radius;
    //vec3 position = vec3(offsetPos, height);
    //vec3 normal = normal(position);
    finalColor = vec4(vec3(height), length(offsetPos) <= radius);
}
