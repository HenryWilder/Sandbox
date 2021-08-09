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

uniform vec3 cameraPosition = vec3(-50.0, 0.0, 0.0);
uniform vec3 cameraTarget = vec3(0.0, 0.0, 0.0);

uniform float c = 10.0; // Light speed can be whatever I want
uniform float G = 6.67*exp(-11.0);

vec3 PrimRay()
{
	vec3 forward = normalize(cameraTarget - cameraPosition);
	vec3 right = cross(forward, vec3(0.0,-1.0,0.0));
	vec3 up = cross(right, forward);
	return normalize(forward + right * (fragTexCoord.x - 0.5) + up * (fragTexCoord.y - 0.5));
}

struct Sphere
{
	vec3 position;
	float radius;
	vec3 color_diffuse;
	vec3 color_emissive;
	float opacity;

	float mass;
};
struct Light
{
	vec3 position;
	vec3 color;
};

uniform Sphere sphere[] =
{
	Sphere(vec3(0.0, 0.0, 0.0), 5.0, vec3(1.0), vec3(-5.0), 1.0, 5.0),
	Sphere(vec3(-10.0, -2.0, 0.0), 2.0, vec3(1.0), vec3(2.0), 1.0, 2.0),
	Sphere(vec3(-10.0, 0.0, 10.0), 1.0, vec3(1.0), vec3(1.0), 1.0, 1.0),
};
uniform int spheres_count = 3;

struct RayHit
{
	int index;
	float dist;
};

RayHit TraceRay(vec3 start, vec3 direction, int ignore)
{
	RayHit hit = RayHit(-1, 10000000000000000000.0);
	
	for (int i = 0; i < spheres_count; ++i)
	{
		if (i != ignore)
		{
			vec3 directionToSphere = normalize(sphere[i].position - start);
			float L = distance(start, sphere[i].position);
			float t_ca = L * max(dot(direction, directionToSphere), 0.0);
			float d = sqrt(L * L - t_ca * t_ca);
			if (d <= sphere[i].radius)
			{
				float t_hc = sqrt(sphere[i].radius * sphere[i].radius - d * d);
				float t0 = t_ca - t_hc;
				if (t0 < hit.dist)
				{
					hit.dist = t0;
					hit.index = i;
				}
			}
		}
	}
	return hit;
}

float DeflectionAngle(float mass, float closestDistance)
{
	return (4.0 * G * mass) / (c * c * closestDistance);
}

uniform int bounce_count = 3;

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	vec3 pixelDirection = PrimRay();

	finalColor = vec4(pixelDirection, 1.0);

	vec3 rayStart = cameraPosition;
	vec3 rayVector = pixelDirection;
	int avoid = -1;
	
	vec3 diffuse = vec3(0.0);
	vec3 emission = vec3(0.0);

	for (int b = 0; b < bounce_count; ++b)
	{
		RayHit hit = TraceRay(rayStart, rayVector, avoid);
		if (hit.index != -1)
		{
			vec3 hitPoint = rayStart + rayVector * hit.dist;
			vec3 normal = normalize(hitPoint - sphere[hit.index].position);
			vec3 reflection = reflect(pixelDirection, normal);

			diffuse += sphere[hit.index].color_diffuse * (sphere[hit.index].color_emissive / distance(hitPoint, hitPoint + reflection * hit.dist));
			if (b == 0) emission = sphere[hit.index].color_emissive;

			rayStart = hitPoint;
			rayVector = reflection;
			avoid = hit.index;
		}
	}
	finalColor = vec4(diffuse + emission, 1.0);
}
