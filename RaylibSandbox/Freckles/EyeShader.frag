#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform sampler2D lightBake;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
const vec3 irisColor = vec3(172,27,25) / vec3(255);

float fragBrightness = 0.0;

void main()
{
    vec3 components = pow(texture(lightBake, fragTexCoord).rgb * 4.0, vec3(2.0));
    float ambientBrightness = components.r;
    float diffuseBrightness = components.g;
    float specularBrightness = components.b;
    fragBrightness = ambientBrightness + diffuseBrightness + specularBrightness;
    
    float brightness = (ambientBrightness + diffuseBrightness * 0.5 + specularBrightness * 0.3) * 0.15;

    vec3 diffuseColor = (brightness > 0.75 ? vec3(1.0) : irisColor * brightness * 1.5);

    finalColor = vec4(diffuseColor, 1.0);
}

