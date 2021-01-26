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
uniform vec2 resolution = vec2(1280, 720);

void main()
{
float x = 1.0/resolution.x;
float y = 1.0/resolution.y;

// Layout:
// R: Survival? 1.0 if true, otherwise subtract 1/n. Floor neighbor pixels below 1.0 when determining survival.
// G: Delta: how many frames since this pixel changed. Upon changing, set to 1.0. Subtract 1/n every frame in which we did not change.
// B: Neighbors: how many neighbors do I have this frame? (n/8)
// A: 

vec4 inputValue = texture2D(texture0, fragTexCoord);
bool self = (int(floor(inputValue.r)) == 1);

int neighbors = 0;
neighbors += int(floor(texture2D(texture0, vec2(fragTexCoord.x - x, fragTexCoord.y - y)).r));
neighbors += int(floor(texture2D(texture0, vec2(fragTexCoord.x,     fragTexCoord.y - y)).r));
neighbors += int(floor(texture2D(texture0, vec2(fragTexCoord.x + x, fragTexCoord.y - y)).r));

neighbors += int(floor(texture2D(texture0, vec2(fragTexCoord.x - x, fragTexCoord.y    )).r));

neighbors += int(floor(texture2D(texture0, vec2(fragTexCoord.x + x, fragTexCoord.y    )).r));

neighbors += int(floor(texture2D(texture0, vec2(fragTexCoord.x - x, fragTexCoord.y + y)).r));
neighbors += int(floor(texture2D(texture0, vec2(fragTexCoord.x,     fragTexCoord.y + y)).r));
neighbors += int(floor(texture2D(texture0, vec2(fragTexCoord.x + x, fragTexCoord.y + y)).r));

bool survival = ((self && (neighbors == 2)) || neighbors == 3);
bool changed = survival != self;
finalColor = vec4(
(survival ? 1.0 : clamp(inputValue.r - 0.016, 0.0, 1.0)), // Survived
(changed ? 1.0 : clamp(inputValue.g - 0.016, 0.0, 1.0)), // Changed
(float(neighbors) / 8.0), // How many neighbors
((survival && !changed) ? clamp(inputValue.a - 0.016, 0.0, 1.0) : 1.0)); // Age
}
