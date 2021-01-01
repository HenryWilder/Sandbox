#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 resolution = vec2(800, 450);

void main() 
{
float x = 1.0/resolution.x;
float y = 1.0/resolution.y;

float value = texture2D(texture0, vec2(fragTexCoord.x, fragTexCoord.y)).x;
vec2 slope = vec2(0.5);
slope.x = texture2D(texture0, vec2(fragTexCoord.x - x, fragTexCoord.y)).x - value;
slope.y = texture2D(texture0, vec2(fragTexCoord.x, fragTexCoord.y - y)).x - value;
vec3 pos = vec3(fragTexCoord, value);
vec3 normal = normalize(vec3(slope, pos.yz));
normal = (normal + vec3(1.0)) / vec3(2.0);
finalColor = vec4(normal, texture2D(texture0, vec2(fragTexCoord.x, fragTexCoord.y)).a);
}
