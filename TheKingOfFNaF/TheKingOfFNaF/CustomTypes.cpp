#include "CustomTypes.h"

unsigned short const& ClockTime::GetDeciseconds()
{
	return m_deciseconds;
}

unsigned short ClockTime::GetSeconds()
{
	return (m_deciseconds / 10u);
}

unsigned char ClockTime::GetMinutes()
{
	return (unsigned char)(GetSeconds() / 60u); // 60 seconds (realtime)
}

unsigned char ClockTime::GetHour()
{
	return (unsigned char)(GetSeconds() / 45u); // 45 seconds (gametime)
}

unsigned short ClockTime::GetWholeHourDeciseconds()
{
	return ((unsigned short)GetHour() * 450u);
}

unsigned short ClockTime::GetDecisecondsSinceHour()
{
	return (GetDeciseconds() - GetWholeHourDeciseconds());
}

void ClockTime::IncrementTime(unsigned short const& amount)
{
	m_deciseconds += amount;
}
