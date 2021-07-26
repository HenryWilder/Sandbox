#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform sampler2D lightMask;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 size = vec2(2048,2048);  // render size (1024,2048)
uniform vec3 lightPos = vec3(-16.0,16.0,16.0);

const float shininess = 16.0;

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
        
        if (true) {
            vec3 reflectDir = reflect(lightDir, fragNormal);
            specAngle = max(dot(reflectDir, viewDir), 0.0);
            // note that the exponent is different here
            specular = pow(specAngle, shininess/4.0);
        }
        else { // this is blinn phong
            vec3 halfDir = normalize(lightDir + viewDir);
            specAngle = max((dot(halfDir, fragNormal) + 1.0), 0.0);
            specular = pow(specAngle, shininess);
        }
    }

    float ambientBrightness = clamp(0.2, 0.0, 1.0);
    float diffuseBrightness = clamp(lambertian * lightPower / dist, 0.0, 1.0);
    float specularBrightness = clamp(specular * lightPower / dist, 0.0, 1.0);

	finalColor = vec4(vec3(ambientBrightness, diffuseBrightness, specularBrightness), 1.0);
    //finalColor = vec4(1.0);
}
