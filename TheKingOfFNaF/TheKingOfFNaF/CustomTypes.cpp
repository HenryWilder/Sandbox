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
	printf("\x1b[0;0HTime: %d:%d.%d\n\nStatuses\n========\nVentilation %s\nLeft  door  %s\nFront vent  %s\nRight door  %s\nRight vent  %s\nFlashlight  %s\nNM BB  %s\n",
		(int)(gameData.time.GetMinutes()), (int)(gameData.time.GetSeconds() % 60), (int)(gameData.time.GetDeciseconds() % 10),
		gameData.ventilationNeedsReset ? "WARNING" : "good   ",
		gameData.doorsClosed[0] ? "closed" : "open  ",
		gameData.doorsClosed[1] ? "closed" : "open  ",
		gameData.doorsClosed[2] ? "closed" : "open  ",
		gameData.doorsClosed[3] ? "closed" : "open  ",
		gameData.flashlight ? "on " : "off",
		gameData.nmBB ? "up  " : "down"
		);
	const char* dispState;
	switch (state)
	{
	case State::Office: dispState = "Office";
		break;
	case State::Camera: dispState = "Camera";
		break;
	case State::Vent: dispState = "Vent  ";
		break;
	case State::Duct: dispState = "Duct  ";
		break;
	default: dispState = "Error ";
		break;
	}
	printf("Gamestate\n=========\nState: %s\n                                 \x1b[1G", dispState);
	if (state == State::Camera)
	{
		const char* camera;

		switch (stateData.cd.camera)
		{
		case Camera::EastHall: camera = "East hall         ";
			break;
		case Camera::Kitchen: camera = "Kitchen           ";
			break;
		case Camera::PartsAndServices: camera = "Parts and services";
			break;
		case Camera::PirateCove: camera = "Pirate cove       ";
			break;
		case Camera::PrizeCounter: camera = "Prize counter     ";
			break;
		case Camera::ShowtimeStage: camera = "Showtime stage    ";
			break;
		case Camera::WestHall: camera = "West hall         ";
			break;
		case Camera::Closet: camera = "Supply closet     ";
			break;
		default: camera = "Error             ";
			break;
		}
		printf("Looking at: %s", camera);
	}
	else if (state == State::Office)
	{
		printf("Yaw: %f", stateData.od.officeYaw);
	}
	printf("\n\n");
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
	gameData.nmBB = false;
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

ColorHSL RGBtoHSL(Color color)
{
	double col[3] = {
		(double)color.r / 255.0,
		(double)color.g / 255.0,
		(double)color.b / 255.0, };

	double cmax;
	int cmaxComp;
	double cmin;

	if (col[0] > col[1]) {
		if (col[0] > col[2]) cmaxComp = 0;
		else cmaxComp = 2; // col[0] < col[2]
	}
	else { // col[0] < col[1]
		if (col[1] > col[2]) cmaxComp = 1;
		else cmaxComp = 2; // col[1] < col[2]
	}

	cmax = col[cmaxComp];
	cmin = min(col[0], min(col[1], col[2]));
	double delta = cmax - cmin;

	double h, s, l;

	// Hue
	if (delta == 0.0) h = 0.0;
	else
	{
		switch (cmaxComp)
		{
		case 0: // Red
			h = 60.0 * ((col[1] - col[2]) / delta);
			break;
		case 1: // Green
			h = 60.0 * (((col[2] - col[0]) / delta) + 2.0);
			break;
		case 2: // Blue
			h = 60.0 * (((col[0] - col[1]) / delta) + 4.0);
			break;
		}
	}

	// Lum
	l = (cmax + cmin) / 2.0;

	// Sat
	if (delta == 0.0) s = 0;
	else
	{
		double temp = 2 * l - 1;
		s = delta / (1 - (temp < 0.0 ? temp * -1.0 : temp));
	}

	// Finished

	return { h,s,l };
}
