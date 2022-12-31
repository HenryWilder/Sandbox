#include <thread>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include "vector_field.h"

constexpr float c = 3.44737343303f; // In units per second
constexpr float c2 = c * c; // c squared
constexpr float c4 = c2 * c2; // c to the power of 4
constexpr float G = 10.0; // Gravitational constant
constexpr float massOfLight = 0.01; // Fudge number for testing

Vector3 SpeedLimited(Vector3 velocity)
{
	if (Vector3LengthSqr(velocity) <= c2)
		return velocity;

	Vector3 direction = Vector3Normalize(velocity);
	Vector3 newVelocity = Vector3Scale(direction, c);
	return newVelocity;
}

// Like a black hole or star
struct MassPoint
{
	Vector3 position;
	Color color;
	float emission; // 0 for base color
	float radius;
	float mass;
};

std::vector<MassPoint> bodies = {
	// Black hole
	//MassPoint{
	//	.position = { 0,2,0 },
	//	.color = MAGENTA,
	//	.emission = 0,
	//	.radius = 0.01f,
	//	.mass = 200.0f
	//},
	// Star 1
	MassPoint{
		.position = { 35,0,0 },
		.color = SKYBLUE,
		.emission = 3000.0f,
		.radius = 16.0f,
		.mass = 10000.0f
	},
	// Star 2
	MassPoint{
		.position = { 0,0,15 },
		.color = RED,
		.emission = 500.0f,
		.radius = 1.0f,
		.mass = 100.0f
	},
	// Star 3
	MassPoint{
		.position = { 0,0,-15 },
		.color = YELLOW,
		.emission = 1000.0f,
		.radius = 3.0f,
		.mass = 1000.0f
	},
	// Planet
	MassPoint{
		.position = { 0,0,-5 },
		.color = ORANGE,
		.emission = 1.0f,
		.radius = 1.0f,
		.mass = 50.0f
	},
};

Vector2 DebugPosition(Vector3 position)
{
	Vector2 returnValue = {
		position.x * 8.0f + 720+360+180,
		position.z * 8.0f + 180
	};
	return returnValue;
}

Vector2 DebugVector(Vector3 direction)
{
	Vector2 returnValue = {
		direction.x * 8.0f,
		direction.z * 8.0f
	};
	return returnValue;
}

float DebugLength(float length)
{
	float returnValue = length * 8.0f;
	return returnValue;
}

void DrawDebugLine(Vector3 startPoint, Vector3 endPoint, Color color)
{
	Vector2 start = DebugPosition(startPoint);
	Vector2 end = DebugPosition(endPoint);
	DrawLineV(start, end, color);
}

Vector3 Vector3Direction(Vector3 p1, Vector3 p2)
{
	return Vector3Normalize(Vector3Subtract(p2, p1));
}

float Force(float mass1, float mass2, float distance)
{
	return (G * mass1 * mass2) / (distance * distance);
}

constexpr float simulationResolution = 0.1f; // dt of simulation
constexpr float cutoffDistance = 8.0f; // "Fog distance"
Color LightTrace(Vector3 startPoint, Vector3 startDirection)
{
	const bool draw = startDirection.y == 0.0f;
	MassPoint* hit = nullptr;
	Vector3 lastPoint = startPoint;
	Vector3 velocity = Vector3Scale(startDirection, simulationResolution * c);
	float distance = 0.0f;
	for (; distance <= cutoffDistance && !hit; distance += simulationResolution * c)
	{
		Vector3 acceleration = Vector3Zero();
		for (MassPoint& body : bodies)
		{
			float distanceToBody = Vector3Distance(lastPoint, body.position);
			if (distanceToBody <= body.radius) hit = &body;
			if (distanceToBody == 0.0f) continue;
			Vector3 directionToBody = Vector3Direction(lastPoint, body.position);
			float force = Force(massOfLight, body.mass, distanceToBody) * simulationResolution;
			Vector3 accelerationDueToBody = Vector3Scale(directionToBody, force);
			acceleration = Vector3Add(acceleration, accelerationDueToBody);
		}
		velocity = Vector3Add(velocity, acceleration);
		velocity = SpeedLimited(velocity);
		Vector3 currentPoint = Vector3Add(lastPoint, velocity);

		if (draw)
			DrawDebugLine(lastPoint, currentPoint, ColorAlpha(BLUE, 0.25f));

		lastPoint = currentPoint;
	}

	Color output = BLACK;
	if (!!hit)
	{
		output = hit->color;
		Vector3 hsv = ColorToHSV(output);
		float multiplier = hit->emission / (distance * distance);
		hsv.z *= multiplier;
		output = ColorFromHSV(hsv.x, hsv.y, hsv.z);
	}
	return output;
}

constexpr float cameraResolution = 2.5f; // Degrees between light rays

// Must come after grid has been initialized or results will be meaningless.
void ApplyGridGravity(float time, float step)
{
	for (int x = 0; x < g_gridWidth; ++x)
	{
		for (int y = 0; y < g_gridWidth; ++y)
		{
			for (int z = 0; z < g_gridWidth; ++z)
			{
				GridPoint& point = g_grid[x][y][z];

				Vector3 lastPoint = point.truePosition;
				Vector3 velocity = Vector3Zero();

				for (float t = 0.0f; t <= time; t += step)
				{
					Vector3 acceleration = Vector3Zero();
					for (MassPoint& body : bodies)
					{
						float distanceToBody = Vector3Distance(lastPoint, body.position);
						if (distanceToBody == 0.0f) continue;
						Vector3 directionToBody = Vector3Direction(lastPoint, body.position);
						float force = Force(1, body.mass, distanceToBody) * step;
						Vector3 accelerationDueToBody = Vector3Scale(directionToBody, force);
						acceleration = Vector3Add(acceleration, accelerationDueToBody);
					}
					velocity = Vector3Add(velocity, acceleration);
					velocity = SpeedLimited(velocity);
					Vector3 currentPoint = Vector3Add(lastPoint, velocity);

					lastPoint = currentPoint;
				}

				point.truePosition = lastPoint;
			}
		}
	}
}

void VectorFieldTest()
{
	InitGrid();
	ApplyGridGravity(0.1f, 0.1f);

	InitWindow(1280, 720, "Relativistic Raytrace");
	SetTargetFPS(60);

	Camera3D camera{};
	camera.position = { -10, 10, 0 };
	camera.target = { 0, 0, 0 };
	camera.up = { 0, 1, 0 };
	camera.fovy = 45;
	camera.type = CAMERA_PERSPECTIVE;
	SetCameraMode(camera, CAMERA_ORBITAL);

	while (!WindowShouldClose())
	{
		UpdateCamera(&camera);

		BeginDrawing();
		ClearBackground(BLACK);

		BeginMode3D(camera);

		for (int x = 0; x < g_gridWidth; ++x)
		{
			for (int y = 0; y < g_gridHeight; ++y)
			{
				for (int z = 0; z < g_gridDepth; ++z)
				{
					Vector3 point = TruePositionAtGridIntegerPosition(x, y, z);
		
					try {
						if (IsGridIntegerPositionWithinBounds(x + 1, y, z))
						{
							Vector3 pointX = TruePositionAtGridIntegerPosition(x + 1, y, z);
							DrawLine3D(point, pointX, RED);
						}
						if (IsGridIntegerPositionWithinBounds(x, y + 1, z))
						{
							Vector3 pointY = TruePositionAtGridIntegerPosition(x, y + 1, z);
							DrawLine3D(point, pointY, GREEN);
						}
						if (IsGridIntegerPositionWithinBounds(x, y, z + 1))
						{
							Vector3 pointZ = TruePositionAtGridIntegerPosition(x, y, z + 1);
							DrawLine3D(point, pointZ, BLUE);
						}
					}
					catch (std::exception e)
					{
						printf("Out of bounds");
					}
		
					DrawPoint3D(point, WHITE);
				}
			}
		}

		for (MassPoint& body : bodies)
		{
			DrawSphere(body.position, body.radius, body.color);
		}

		EndMode3D();

		DrawFPS(0,0);
		EndDrawing();
	}
	CloseWindow();
}

int main()
{
	VectorFieldTest();
	return 0;

	InitWindow(720*2, 720, "Relativistic Raytrace");
	SetTargetFPS(60);

	Rectangle topDown = { 720+360, 0, 360, 360 };

	Vector3 up = { 0,1,0 };

	Camera3D camera{};
	camera.position = { -10.0f, 0, 0 };
	camera.target = { 0, 0, 0 };
	camera.up = up;
	camera.fovy = 70;
	camera.type = CAMERA_PERSPECTIVE;
	SetCameraMode(camera, CAMERA_FREE);

	while (!WindowShouldClose())
	{
		UpdateCamera(&camera);

		Vector2 cameraPosition2D = { camera.position.x, camera.position.z };
		Vector2 cameraTarget2D = { camera.target.x, camera.target.z };
		Vector3 cameraDirection = Vector3Direction(camera.position, camera.target);
		float cameraAngle = Vector2Angle(cameraPosition2D, cameraTarget2D) + 90.0f;

		BeginDrawing();
		ClearBackground(BLACK);

		for (float thetaY = -45.0f; thetaY <= 45.0f; thetaY += cameraResolution)
		{
			Quaternion angleY = QuaternionFromAxisAngle(Vector3CrossProduct(up, cameraDirection), thetaY * DEG2RAD);
			for (float theta = -45.0f; theta <= 45.0f; theta += cameraResolution)
			{
				Quaternion angle = QuaternionFromAxisAngle(up, theta * DEG2RAD);
				Color color = LightTrace(camera.position, Vector3RotateByQuaternion(cameraDirection, QuaternionAdd(angle, angleY)));
				DrawRectangle((int)(720.0f + theta * 6.0f), (int)(360.0f + thetaY * 6.0f), 6 * cameraResolution, 6 * cameraResolution, color);
			}
		}

		for (MassPoint& body : bodies)
		{
			DrawCircleV(DebugPosition(body.position), DebugLength(body.radius), body.color);
		}

		DrawCircleSector(DebugPosition(camera.position), 5.0f, cameraAngle - 45.0f, cameraAngle + 45.0f, 1, WHITE);

		DrawRectangleLinesEx(topDown, 3, DARKGREEN);
		DrawFPS(0, 0);
		EndDrawing();
	}

	CloseWindow();
}