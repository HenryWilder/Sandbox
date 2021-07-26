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
const vec3 glow = vec3(121,23,207) / vec3(255);
const vec3 shine = vec3(32,142,249) / vec3(255);
const vec3 skin = (vec3(57,75,68) / vec3(255)) * 0.75;
const vec3 skinShine = vec3(81,106,96) / vec3(255);
const vec3 skinShade = (vec3(50,65,59) / vec3(255)) * 0.75;

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
    
    float shineStrength = clamp(pow(fragBrightness, 3.0) * 0.5, 0.0, 1.0);
    vec3 emissiveColor = mix(glow, shine, shineStrength);
    
    float brightness = (ambientBrightness + diffuseBrightness * 0.5 + specularBrightness * 0.3) * 0.15;

    vec3 diffuseColor = (brightness < 0.125 ? skinShade : mix(skin * clamp(brightness * 10.0, 0.5, 1.5), skinShine, 4.0 * clamp(brightness - 0.75, 0.0, 1.0)));

    finalColor = vec4(mix(clamp(diffuseColor, 0.0, 1.0), clamp(emissiveColor, 0.0, 1.0), clamp(freckle, 0.0, 1.0)), 1.0);
    //finalColor = vec4(fragNormal,1.0);
    //finalColor = vec4(fragPosition,1.0);
    //finalColor = vec4(vec3(fragBrightness), 1.0);
}

