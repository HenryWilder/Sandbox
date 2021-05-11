#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;
in vec3 vertPos;

// Input uniform values
uniform sampler2D texture0;
uniform sampler2D textureEmis;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 size = vec2(1024,2048);  // render size
uniform vec3 lightPos = vec3(-5.0,5.0,5.0);

const float shininess = 16.0;

const vec3 ramp0 = vec3(116,26,248) / vec3(255);
const vec3 ramp1 = vec3(38,144,252) / vec3(255);
const vec3 skin0 = vec3(20,20,20) / vec3(255);
const vec3 subskin = vec3(1.0,0.0,0.0);

const float lightPower = 150.0;
float fragBrightness = 0.0;

void main()
{
    // Fragment brightness

    vec3 lightDir = lightPos - fragPosition;
    float dist = length(lightDir);
    dist *= dist;
    lightDir = normalize(lightDir);
    
    float lambertian = max((1.0 + dot(lightDir, fragNormal)) * 0.5, 0.0);
    float specular = 0.0;
    float specAngle = 0.0;

    if (lambertian > 0.0)
    {
        vec3 viewDir = normalize(-fragPosition);
        
        // this is blinn phong
        vec3 halfDir = normalize(lightDir + viewDir);
        specAngle = max((dot(halfDir, fragNormal) + 1.0), 0.0);
        specular = pow(specAngle, shininess);

        if (true) {
            vec3 reflectDir = reflect(lightDir, fragNormal);
            specAngle = max(dot(reflectDir, viewDir), 0.0);
            // note that the exponent is different here
            specular = pow(specAngle, shininess/4.0);
        }
    }

    fragBrightness = (lambertian * lightPower / dist) +
                     (specular   * lightPower / dist);

    // Texel color fetching from texture sampler
    vec3 emissiveColor = mix(ramp0, ramp1, clamp(fragBrightness, 0.0, 1.0));

    vec3 skinColor = skin0 * (
                     (0.1) +
                     (lambertian * lightPower / dist) +
                     (specular   * lightPower / dist));

    vec3 heatedColor = subskin * (0.02 * lambertian * lightPower / dist);

    vec3 frecklColor = ((emissiveColor * pow(lambertian,1/(shininess/2))) + pow(specAngle,shininess*8.0) * lightPower / dist);

    finalColor = vec4(mix(skinColor + heatedColor, frecklColor, texture(textureEmis, fragTexCoord).r), 1.0);
}

