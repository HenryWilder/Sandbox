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

struct Plane
{
	vec3 position;
	vec3 normal;
	vec3 color_diffuse;
	vec3 color_emissive;
};
uniform Plane planes[] =
{
	Plane(vec3(0.0,-5.0,0.0), vec3(0.0,1.0,0.0), vec3(0.0), vec3(0.0)),
	Plane(vec3(0.0,1000.0,0.0), vec3(0.0,-1.0,0.0), vec3(1000000.0), vec3(0.0)),
	Plane(vec3(1000.0,0.0,0.0), vec3(-1.0,0.0,0.0), vec3(100000.0), vec3(0.0)),
	Plane(vec3(-1000.0,0.0,0.0), vec3(1.0,0.0,0.0), vec3(100000.0), vec3(0.0)),
};
uniform int planes_count = 2;

struct Sphere
{
	vec3 position;
	float radius;
	vec3 color_diffuse;
	vec3 color_emissive;
	float opacity;
};

uniform Sphere sphere[] =
{
	Sphere(vec3(0.0, 0.0, 0.0), 5.0, vec3(1.0), vec3(10000000.0), 1.0),
	Sphere(vec3(-10.0, -2.0, 0.0), 2.0, vec3(100.0,0.0,0.0), vec3(0.0), 1.0),
	Sphere(vec3(-10.0, 0.0, 10.0), 1.0, vec3(0.0,100.0,0.0), vec3(0.0), 0.0),
	Sphere(vec3(-20.0, 0.0, 0.0), 5.0, vec3(0.0,0.0,100.0), vec3(0.0), 1.0),
	Sphere(vec3(5.0,10.0,0.0), 4.0, vec3(0.0), vec3(0.0), 1.0),
};
uniform int spheres_count = 5;

struct RayHit
{
	int type; // 0 = none, 1 = plane, 2 = sphere
	int index;
	float dist;
};

RayHit TraceRay(vec3 start, vec3 direction, int ignoreType, int ignoreIndex)
{
	RayHit hit = RayHit(0, -1, 10000000000000000000.0);
	
	// Planes
	for (int i = 0; i < planes_count; ++i)
	{
		if (!(ignoreType == 1 && i == ignoreIndex))
		{
			float projectedDirectionDot = dot(direction, -planes[i].normal);
			if (projectedDirectionDot >= 0.0) // Can't calculate if lines diverge
			{
				float planeToSourceDist = distance(start, planes[i].position);
				float shortestDistToPlane = dot(planes[i].position - start, -planes[i].normal);
				float distanceToHitPoint = shortestDistToPlane / projectedDirectionDot;
				if (distanceToHitPoint < hit.dist)
					hit = RayHit(1,i,distanceToHitPoint);
			}
		}
	}
	// Spheres
	for (int i = 0; i < spheres_count; ++i)
	{
		if (!(ignoreType == 2 && i == ignoreIndex))
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
					hit = RayHit(2,i,t0);
			}
		}
	}
	return hit;
}

uniform int bounce_count = 4;

void main()
{
	float x = 1.0/resolution.x;
	float y = 1.0/resolution.y;

	vec3 pixelDirection = PrimRay();

	finalColor = vec4(pixelDirection, 1.0);

	vec3 rayStart = cameraPosition;
	vec3 rayVector = pixelDirection;
	int avoid_type = 0;
	int avoid_index = -1;
	
	vec3 diffuse = vec3(0.0);
	vec3 emiss = vec3(0.0);

	float totalDistance = 0.0;

	for (int b = 0; b < bounce_count; ++b)
	{
		RayHit hit = TraceRay(rayStart, rayVector, avoid_type, avoid_index);
		if (hit.type != 0)
		{
			vec3 hitPoint = rayStart + rayVector * hit.dist;

			totalDistance += hit.dist;
			float dist2 = totalDistance * totalDistance;

			if (hit.type == 1)
			{
				rayVector = reflect(pixelDirection, planes[hit.index].normal);

				diffuse += planes[hit.index].color_diffuse / dist2;
				emiss += planes[hit.index].color_emissive / dist2;
			}
			else if (hit.type == 2)
			{
				rayVector = reflect(pixelDirection, normalize(hitPoint - sphere[hit.index].position));

				diffuse += sphere[hit.index].color_diffuse / dist2;
				emiss += sphere[hit.index].color_emissive / dist2;
			}

			rayStart = hitPoint;
			avoid_type = hit.type;
			avoid_index = hit.index;
		}
	}
	finalColor = vec4(diffuse * emiss, 1.0);
}
