#include "CustomTypes.h"

unsigned short const& ClockTime::GetDeciseconds() const
{
	return m_deciseconds;
}

unsigned short ClockTime::GetSeconds() const
{
	return (m_deciseconds / 10u);
}

unsigned char ClockTime::GetMinutes() const
{
	return (unsigned char)(GetSeconds() / 60u); // 60 seconds (realtime)
}

unsigned char ClockTime::GetHour() const
{
	return (unsigned char)(GetSeconds() / 45u); // 45 seconds (gametime)
}

unsigned short ClockTime::GetWholeHourDeciseconds() const
{
	return ((unsigned short)GetHour() * 450u);
}

unsigned short ClockTime::GetDecisecondsSinceHour() const
{
	return (GetDeciseconds() - GetWholeHourDeciseconds());
}

void ClockTime::UpdateTime(unsigned short const& newTime)
{
	if (newTime > m_deciseconds)
	{
		m_deciseconds = newTime;
		m_pingsSinceChange = 0;
	}
	else ++m_pingsSinceChange;
}

int const& ClockTime::GetPingsSinceChange()
{
	return m_pingsSinceChange;
}

unsigned char Color::Gray() const
{
	return (unsigned char)(((int)r + (int)g + (int)b) / 3);
}

unsigned char Color::RedDev() const
{
	int distFromMean = (r - Gray());
	return (unsigned char)sqrt((distFromMean * distFromMean) / 3);
}

unsigned char Color::GreenDev() const
{
	int distFromMean = (g - Gray());
	return (unsigned char)sqrt((distFromMean * distFromMean) / 3);
}

unsigned char Color::BlueDev() const
{
	int distFromMean = (b - Gray());
	return (unsigned char)sqrt((distFromMean * distFromMean) / 3);
}

CNorm Color::Normal() const
{
	CNorm normal; // Note that this will always be [0-1]

	normal.r = (double)r / 255.0;
	normal.g = (double)g / 255.0;
	normal.b = (double)b / 255.0;

	double length = sqrt(((normal.r * normal.r) + (normal.g * normal.g) + (normal.b * normal.b))); // a^2 + b^2 + c^2 = d^2

	normal.r /= length;
	normal.g /= length;
	normal.b /= length;

	return normal;
}

void GameState::DisplayData()
{
	printf("\x1b[0;0HTime: %d:%d.%d\n\nStatuses\n========\nVentilation %s\nLeft  door  %s\nFront vent  %s\nRight door  %s\nRight vent  %s\nFlashlight  %s\n\n",
		(int)(gameData.time.GetMinutes()), (int)(gameData.time.GetSeconds()%60), (int)(gameData.time.GetDeciseconds()%10),
		gameData.ventilationNeedsReset ? "WARNING" : "good   ",
		gameData.doorsClosed[0] ? "closed" : "open  ",
		gameData.doorsClosed[1] ? "closed" : "open  ",
		gameData.doorsClosed[2] ? "closed" : "open  ",
		gameData.doorsClosed[3] ? "closed" : "open  ",
		gameData.flashlight ? "on " : "off"
		);
	const char* dispState;
	switch (state)
	{
	case State::Office:
		dispState = "Office";
			break;
	case State::Camera:
		dispState = "Camera";
			break;
	case State::Vent:
		dispState = "Vent  ";
			break;
	case State::Duct:
		dispState = "Duct  ";
		break;
	default:
		dispState = "Error ";
		break;
	}
	printf("Gamestate\n=========\nState: %s\n", dispState);
	if (state == State::Camera)
	{
		const char* camera;

		switch (stateData.cd.camera)
		{
		case Camera::EastHall:
			camera = "East hall         ";
			break;
		case Camera::Kitchen:
			camera = "Kitchen           ";
			break;
		case Camera::PartsAndServices:
			camera = "Parts and services";
			break;
		case Camera::PirateCove:
			camera = "Pirate cove       ";
			break;
		case Camera::PrizeCounter:
			camera = "Prize counter     ";
			break;
		case Camera::ShowtimeStage:
			camera = "Showtime stage    ";
			break;
		case Camera::WestHall:
			camera = "West hall         ";
			break;
		case Camera::Closet:
			camera = "Supply closet     ";
			break;
		default:
			camera = "Error             ";
			break;
		}
		printf("Looking at: %s\n", camera);
	}
	else if (state == State::Office)
	{
		printf("Yaw: %d\n", stateData.od.officeYaw);
	}
	else printf("                                 \n");
	printf("\n");
}

void GameState::Init()
{
	state = State::Office;
	stateData.cd.camera = Camera::WestHall;
	gameData.doorsClosed[0] = false;
	gameData.doorsClosed[1] = false;
	gameData.doorsClosed[2] = false;
	gameData.doorsClosed[3] = false;
	gameData.time = ClockTime();
	gameData.ventilationNeedsReset = false;
	gameData.flashlight = false;
}

// Better for determining how close a color is to another, regardless of the scale. (brightness/darkness)
double CDot(CNorm a, CNorm b)
{
	CNorm c;
	c.r = a.r * b.r;
	c.g = a.g * b.g;
	c.b = a.b * b.b;
	double dot;
	dot = c.r + c.g + c.b;
	return dot;
}

double CDot(Color a, Color b)
{
	return CDot(a.Normal(), b.Normal());
}
