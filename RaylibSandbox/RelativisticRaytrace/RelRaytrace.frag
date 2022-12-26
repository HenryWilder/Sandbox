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

const int planeCountMax = 5;
uniform vec3 planePositions[planeCountMax];
uniform vec3 planeNormals[planeCountMax];
uniform vec3 planeDiffuseColors[planeCountMax];
uniform vec3 planeEmissiveColors[planeCountMax];
uniform int planeCount = 0;

const int sphereCountMax = 5;
uniform vec3 spherePositions[sphereCountMax];
uniform float sphereRadii[sphereCountMax];
uniform vec3 sphereDiffuseColors[sphereCountMax];
uniform vec3 sphereEmissiveColors[sphereCountMax];
uniform int sphereCount = 0;

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
	for (int i = 0; i < min(planeCount, planeCountMax); ++i)
	{
		if (!(ignoreType == 1 && i == ignoreIndex))
		{
			float projectedDirectionDot = dot(direction, -planeNormals[i]);
			if (projectedDirectionDot >= 0.0) // Can't calculate if lines diverge
			{
				float planeToSourceDist = distance(start, planePositions[i]);
				float shortestDistToPlane = dot(planePositions[i] - start, -planeNormals[i]);
				float distanceToHitPoint = shortestDistToPlane / projectedDirectionDot;
				if (distanceToHitPoint < hit.dist)
					hit = RayHit(1,i,distanceToHitPoint);
			}
		}
	}
	// Spheres
	for (int i = 0; i < min(sphereCount, sphereCountMax); ++i)
	{
		if (!(ignoreType == 2 && i == ignoreIndex))
		{
			vec3 directionToSphere = normalize(spherePositions[i] - start);
			float L = distance(start, spherePositions[i]);
			float t_ca = L * max(dot(direction, directionToSphere), 0.0);
			float d = sqrt(L * L - t_ca * t_ca);
			if (d <= sphereRadii[i])
			{
				float t_hc = sqrt(sphereRadii[i] * sphereRadii[i] - d * d);
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
	if (planeCount == 0 && sphereCount == 0)
		discard;

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
				rayVector = reflect(pixelDirection, planeNormals[hit.index]);

				diffuse += planeDiffuseColors[hit.index] / dist2;
				emiss += planeEmissiveColors[hit.index] / dist2;
			}
			else if (hit.type == 2)
			{
				rayVector = reflect(pixelDirection, normalize(hitPoint - spherePositions[hit.index]));

				diffuse += sphereDiffuseColors[hit.index] / dist2;
				emiss += sphereEmissiveColors[hit.index] / dist2;
			}

			rayStart = hitPoint;
			avoid_type = hit.type;
			avoid_index = hit.index;
		}
	}
	finalColor = vec4(diffuse * emiss, 1.0);
}
