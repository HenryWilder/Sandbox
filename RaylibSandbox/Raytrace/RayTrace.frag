#version 330

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;
in vec4 fragColor;

// Input uniform heights
uniform sampler2D texture0;
uniform vec4 colDiffuse;

// Output fragment color
out vec4 finalColor;

// NOTE: Add here your custom variables
uniform vec2 resolution = vec2(720, 720);

uniform vec3 cameraPosition = vec3(-20.0, 0.0, 0.0);
uniform vec3 cameraDirection = vec3(1.0, 0.0, 0.0);

vec3 PrimRay()
{
	vec3 ray = vec3((fragTexCoord - 0.5) * 2.0, 0.0);
	return normalize(ray);
}

struct Sphere
{
	vec3 position;
	float radius;
	vec3 color_diffuse;
	vec3 color_emissive;
	float opacity;
};
struct Light
{
	vec3 position;
	vec3 color;
};

uniform Sphere sphere[] = { Sphere(vec3(0.0), 10.0, vec3(1.0), vec3(0.0), 1.0) };
uniform Light light[] = { Light(vec3(16.0, 16.0, 16.0), vec3(10.0)) };

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	vec3 pixelDirection = PrimRay();

	float shortestDist = 10000000000000000000.0;
	int nearestIndex = -1;

	for (int i = 0; i < 1; ++i)
	{
		vec3 directionToSphere = normalize(sphere[i].position - cameraPosition);
		float L = distance(cameraPosition, sphere[i].position);
		float t_ca = L * dot(pixelDirection, directionToSphere);
		float d = sqrt(L * L - t_ca * t_ca);
		if (d <= sphere[i].radius)
		{
			float t_hc = sqrt(sphere[i].radius * sphere[i].radius - d * d);
			float t0 = t_ca - t_hc;
			if (t0 < shortestDist)
			{
				shortestDist = t0;
				nearestIndex = i;
			}
		}
	}
	if (nearestIndex != -1)
	{
		//vec3 P = cameraPosition + pixelDirection * shortestDist;
		finalColor = vec4(sphere[nearestIndex].color_diffuse, 1.0);
	}
	else
	{
		finalColor = vec4(1.0,0.0,1.0, 1.0);
	}
}
