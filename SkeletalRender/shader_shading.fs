#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 resolution = vec2(800, 450);

float sampleHeightmap(vec2 samplePos);
float sampleHeightmap(vec2 samplePos)
{
    return texture2D(texture0, samplePos).x;
}

void main() 
{
float x = 1.0/resolution.x;
float y = 1.0/resolution.y;

//// Sphere gen
//float radius = 50.0;
//vec2 pos = vec2(fragTexCoord.x / x, fragTexCoord.y / y);
//vec2 offsetPos = pos - (resolution / 2);
//float height = sqrt((radius * radius) - ((offsetPos.x * offsetPos.x) + (offsetPos.y * offsetPos.y))) / radius;
////finalColor = vec4(vec3(height), length(offsetPos) <= radius);
//vec3 position = vec3(offsetPos, height);
//vec3 normal = normalize(position);

// Normal map
//float height = texture2D(texture0, vec2(fragTexCoord.x, fragTexCoord.y)).x;
float height = sampleHeightmap(fragTexCoord);
vec3 slope = vec3(sampleHeightmap(vec2(fragTexCoord.x - x, fragTexCoord.y)) - height, sampleHeightmap(vec2(fragTexCoord.x, fragTexCoord.y - y)) - height, height);
//slope.z = length(slope.xy);
vec3 position = vec3(fragTexCoord, height);
vec3 normal = normalize(slope);
//slope.y *= -1;
//normal = (normal + vec3(1.0)) / vec3(2.0); // For compressing to 0..1

// vec3 lightPosition = vec3(400.0,400.0,400.0);
// vec3 cameraPosition = vec3(400.0,0.0,0.0);

// Shading
vec3 lightDirection = normalize(lightPosition - position);
//vec3 viewDirection = normalize(position - cameraPosition);
//vec3 reflectionNormal = vec3(2.0) * (lightDirection * normal) * normal - lightDirection;
//float lum = 8.0 * dot((lightDirection * normal), (reflectionNormal * viewDirection));
float lum = max(dot(normal, lightDirection), 0.0);
//float dist = distance(lightPosition, position);
//lum = 100.0 * lum / (dist * dist);

//vec4 lightness = vec4(vec3(lum), texture2D(texture0, vec2(fragTexCoord.x, fragTexCoord.y)).a);

finalColor = vec4(vec3(lum) * height, texture2D(texture0, vec2(fragTexCoord.x, fragTexCoord.y)).a);
//finalColor = vec4(normal, length(offsetPos) <= radius);
//finalColor = vec4(vec3(height), 1.0);
}
