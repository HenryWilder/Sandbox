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

float edge = 0.0;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-1 * x ), fragTexCoord.y + (-1 * y ))).x;
edge += texture2D(texture0, vec2(fragTexCoord.x,             fragTexCoord.y + (-1 * y ))).x;
edge += texture2D(texture0, vec2(fragTexCoord.x + (   x   ), fragTexCoord.y + (-1 * y ))).x;

edge += texture2D(texture0, vec2(fragTexCoord.x + (-1 * x ), fragTexCoord.y            )).x;
edge += texture2D(texture0, vec2(fragTexCoord.x,             fragTexCoord.y            )).x;
edge += texture2D(texture0, vec2(fragTexCoord.x + (   x   ), fragTexCoord.y            )).x;

edge += texture2D(texture0, vec2(fragTexCoord.x + (-1 * x ), fragTexCoord.y + (   y   ))).x;
edge += texture2D(texture0, vec2(fragTexCoord.x,             fragTexCoord.y + (   y   ))).x;
edge += texture2D(texture0, vec2(fragTexCoord.x + (   x   ), fragTexCoord.y + (   y   ))).x;

finalColor = vec4(vec3(0.961,0.529,0.258), edge * 0.111111111);
}
