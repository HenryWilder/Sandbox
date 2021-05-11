#version 330

// Input vertex attributes (from vertex shader)
in vec3 fragPosition;
in vec2 fragTexCoord;
in vec4 fragColor;
in vec3 fragNormal;

// Input uniform values
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 size = vec2(1280,720);  // render size

const float samples = 10.0;  // pixels per axis; higher = bigger glow, worse performance
const float quality = 2.5;  // lower = smaller glow, better quality

void main()
{
    vec4 sum = vec4(0);
    vec2 sizeFactor = vec2(1) / size * quality;

    const int range = (int(samples) - 1) / 2; // should be = (samples - 1)/2;

    for (int x = -range; x <= range; x++)
    {
        for (int y = -range; y <= range; y++)
        {
            vec2 samplePos = fragTexCoord + vec2(x, y) * sizeFactor;
            sum += texture(texture0, samplePos) * distance(fragTexCoord,samplePos);
        }
    }

    finalColor = (sum / (samples * samples)) + texture(texture0, fragTexCoord) * colDiffuse;
}

