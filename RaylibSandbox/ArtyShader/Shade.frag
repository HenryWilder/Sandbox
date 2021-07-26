#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 normal;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 resolution = vec2(1280, 720);
uniform vec2 lightPos = vec2(0,0);

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

    vec3 normal = normalize(normalInterp);
    vec3 lightDir = lightPos - vertPos;
    float distance = length(lightDir);
    distance = distance * distance;
    lightDir = normalize(lightDir);
    
    float lambertian = max(dot(lightDir, normal), 0.0);
    float specular = 0.0;
    
    if (lambertian > 0.0) {
    
      vec3 viewDir = normalize(-vertPos);
    
      // this is blinn phong
      vec3 halfDir = normalize(lightDir + viewDir);
      float specAngle = max(dot(halfDir, normal), 0.0);
      specular = pow(specAngle, shininess);
         
      // this is phong (for comparison)
      if (mode == 2) {
        vec3 reflectDir = reflect(-lightDir, normal);
        specAngle = max(dot(reflectDir, viewDir), 0.0);
        // note that the exponent is different here
        specular = pow(specAngle, shininess/4.0);
      }
    }
    vec3 colorLinear = ambientColor +
                       diffuseColor * lambertian * lightColor * lightPower / distance +
                       specColor * specular * lightColor * lightPower / distance;
    // apply gamma correction (assume ambientColor, diffuseColor and specColor
    // have been linearized, i.e. have no gamma correction in them)
    vec3 colorGammaCorrected = pow(colorLinear, vec3(1.0 / screenGamma));
    // use the gamma corrected color in the fragment
    gl_FragColor = vec4(colorGammaCorrected, 1.0);

	finalColor = vec4(1.0);
}
