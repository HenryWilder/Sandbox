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
edge += texture2D(texture0, vec2(fragTexCoord.x + (-3 * x ), fragTexCoord.y + (-3 * y ))).x;//*0.15;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-2 * x ), fragTexCoord.y + (-3 * y ))).x;//*0.28;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-1 * x ), fragTexCoord.y + (-3 * y ))).x;//*0.37;
edge += texture2D(texture0, vec2(fragTexCoord.x,             fragTexCoord.y + (-3 * y ))).x;//*0.40;
edge += texture2D(texture0, vec2(fragTexCoord.x + (   x   ), fragTexCoord.y + (-3 * y ))).x;//*0.37;
edge += texture2D(texture0, vec2(fragTexCoord.x + ( 2 * x ), fragTexCoord.y + (-3 * y ))).x;//*0.28;
edge += texture2D(texture0, vec2(fragTexCoord.x + ( 3 * x ), fragTexCoord.y + (-3 * y ))).x;//*0.15;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-3 * x ), fragTexCoord.y + (-2 * y ))).x;//*0.28;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-2 * x ), fragTexCoord.y + (-2 * y ))).x;//*0.43;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-1 * x ), fragTexCoord.y + (-2 * y ))).x;//*0.55;
edge += texture2D(texture0, vec2(fragTexCoord.x,             fragTexCoord.y + (-2 * y ))).x;//*0.60;
edge += texture2D(texture0, vec2(fragTexCoord.x + (   x   ), fragTexCoord.y + (-2 * y ))).x;//*0.55;
edge += texture2D(texture0, vec2(fragTexCoord.x + ( 2 * x ), fragTexCoord.y + (-2 * y ))).x;//*0.43;
edge += texture2D(texture0, vec2(fragTexCoord.x + ( 3 * x ), fragTexCoord.y + (-2 * y ))).x;//*0.28;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-3 * x ), fragTexCoord.y + (-1 * y ))).x;//*0.37;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-2 * x ), fragTexCoord.y + (-1 * y ))).x;//*0.55;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-1 * x ), fragTexCoord.y + (-1 * y ))).x;//*0.72;
edge += texture2D(texture0, vec2(fragTexCoord.x,             fragTexCoord.y + (-1 * y ))).x;//*0.80;
edge += texture2D(texture0, vec2(fragTexCoord.x + (   x   ), fragTexCoord.y + (-1 * y ))).x;//*0.72;
edge += texture2D(texture0, vec2(fragTexCoord.x + ( 2 * x ), fragTexCoord.y + (-1 * y ))).x;//*0.55;
edge += texture2D(texture0, vec2(fragTexCoord.x + ( 3 * x ), fragTexCoord.y + (-1 * y ))).x;//*0.37;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-3 * x ), fragTexCoord.y            )).x;//*0.40;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-2 * x ), fragTexCoord.y            )).x;//*0.60;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-1 * x ), fragTexCoord.y            )).x;//*0.80;
edge += texture2D(texture0, vec2(fragTexCoord.x,             fragTexCoord.y            )).x;//*1.00;
edge += texture2D(texture0, vec2(fragTexCoord.x + (   x   ), fragTexCoord.y            )).x;//*0.80;
edge += texture2D(texture0, vec2(fragTexCoord.x + ( 2 * x ), fragTexCoord.y            )).x;//*0.60;
edge += texture2D(texture0, vec2(fragTexCoord.x + ( 3 * x ), fragTexCoord.y            )).x;//*0.40;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-3 * x ), fragTexCoord.y + (   y   ))).x;//*0.37;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-2 * x ), fragTexCoord.y + (   y   ))).x;//*0.55;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-1 * x ), fragTexCoord.y + (   y   ))).x;//*0.72;
edge += texture2D(texture0, vec2(fragTexCoord.x,             fragTexCoord.y + (   y   ))).x;//*0.80;
edge += texture2D(texture0, vec2(fragTexCoord.x + (   x   ), fragTexCoord.y + (   y   ))).x;//*0.72;
edge += texture2D(texture0, vec2(fragTexCoord.x + ( 2 * x ), fragTexCoord.y + (   y   ))).x;//*0.55;
edge += texture2D(texture0, vec2(fragTexCoord.x + ( 3 * x ), fragTexCoord.y + (   y   ))).x;//*0.37;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-3 * x ), fragTexCoord.y + ( 2 * y ))).x;//*0.28;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-2 * x ), fragTexCoord.y + ( 2 * y ))).x;//*0.43;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-1 * x ), fragTexCoord.y + ( 2 * y ))).x;//*0.55;
edge += texture2D(texture0, vec2(fragTexCoord.x,             fragTexCoord.y + ( 2 * y ))).x;//*0.60;
edge += texture2D(texture0, vec2(fragTexCoord.x + (   x   ), fragTexCoord.y + ( 2 * y ))).x;//*0.55;
edge += texture2D(texture0, vec2(fragTexCoord.x + ( 2 * x ), fragTexCoord.y + ( 2 * y ))).x;//*0.43;
edge += texture2D(texture0, vec2(fragTexCoord.x + ( 3 * x ), fragTexCoord.y + ( 2 * y ))).x;//*0.28;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-3 * x ), fragTexCoord.y + ( 3 * y ))).x;//*0.15;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-2 * x ), fragTexCoord.y + ( 3 * y ))).x;//*0.28;
edge += texture2D(texture0, vec2(fragTexCoord.x + (-1 * x ), fragTexCoord.y + ( 3 * y ))).x;//*0.37;
edge += texture2D(texture0, vec2(fragTexCoord.x,             fragTexCoord.y + ( 3 * y ))).x;//*0.40;
edge += texture2D(texture0, vec2(fragTexCoord.x + (   x   ), fragTexCoord.y + ( 3 * y ))).x;//*0.37;
edge += texture2D(texture0, vec2(fragTexCoord.x + ( 2 * x ), fragTexCoord.y + ( 3 * y ))).x;//*0.28;
edge += texture2D(texture0, vec2(fragTexCoord.x + ( 3 * x ), fragTexCoord.y + ( 3 * y ))).x;//*0.15;

finalColor = vec4(vec3(0.258,0.529,0.961), edge);
}
