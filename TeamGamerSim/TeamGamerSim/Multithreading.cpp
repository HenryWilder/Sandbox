#include "Multithreading.h"

bool g_awake = true;

void Protect()
{
	while (g_awake)
	{
		Sleep(15);
		POINT p = GetMousePos();
		if (p.x == 0 || p.y == 0)
		{
			g_awake = false;
			break;
		}
	}
}

void Perceive()
{
}

void Think()
{
}

void Feel()
{
}
typedef void (*Subclass)(void);
void Play()
{
	//ChooseClass((int)Class::type::Spy);
	g_myClass = Class::type::Spy;
	Subclass playStyle = Class::Spy::CloakAndRecon;

	while (g_awake)
	{
		playStyle();
		Sleep(2);
	}
}

void CreateThreads()
{
	//std::thread sense(Perceive); // Allow us to see our environment
	//std::thread brain(Think); // Look ahead at where our actions will put us
	//std::thread heart(Feel); // Care how other players feel about us
	std::thread hands(Play); // Interact with the world

	Protect(); // Keep the user safe

	// If guard determines we have failed, stop the program
	//sense.join();
	//brain.join();
	//heart.join();
	hands.join();
}
