#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform values
uniform sampler2D texture0;
uniform sampler2D mask;

// Output fragment color
out vec4 finalColor;

void main()
{
    vec4 maskColor = texture(texture0, fragTexCoord);
    if ((maskColor.r + maskColor.g + maskColor.b) < 0.25) discard;
    vec4 texelColor = texture(texture0, fragTexCoord);

    //finalColor = texelColor*maskColor;
    finalColor = vec4(1,0,1,1);
}
