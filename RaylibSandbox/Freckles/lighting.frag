#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform sampler2D lightAccum;
uniform sampler2D freckleMask;
uniform sampler2D lightBake;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
const vec3 glow = vec3(116,26,248) / vec3(255);
const vec3 shine = vec3(38,144,252) / vec3(255);
const vec3 skin = vec3(20,20,20) / vec3(255);
const vec3 subskin = vec3(1.0,0.0,0.0);

float fragBrightness = 0.0;

void main()
{
    vec3 components = pow(texture(lightBake, fragTexCoord).rgb * 4.0, vec3(2.0));
    float ambientBrightness = components.r;
    float diffuseBrightness = components.g;
    float specularBrightness = components.b;
    fragBrightness = ambientBrightness + diffuseBrightness + specularBrightness;
    
    vec3 accum = texture(lightAccum, fragTexCoord).rgb;
    float freckle = texture(freckleMask, fragTexCoord).r;
    
    float shineStrength = clamp(pow(fragBrightness,2.0), 0.0, 1.0);
    vec3 emissiveColor = mix(glow, shine, shineStrength);
    
    vec3 skinColor = skin;
    vec3 heatedColor = subskin * diffuseBrightness;
    vec3 diffuseColor = skinColor * (ambientBrightness + diffuseBrightness * 0.5 + specularBrightness * 0.3) * 0.5;

    finalColor = vec4(mix(clamp(diffuseColor, 0.0, 1.0), clamp(emissiveColor, 0.0, 1.0), clamp(freckle, 0.0, 1.0)), 1.0);
}

