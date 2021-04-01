#include <iostream>
#include <fstream>
#include <bitset>

int i = 1 << 16;

struct Date
{
	enum WeekdayName
	{
		Monday,
		Tuesday,
		Wednesday,
		Thursday,
		Friday,
		Saturday,
		Sunday,
	};
	enum MonthName
	{
		January,
		Febuary,
		March,
		April,
		May,
		June,
		July,
		August,
		September,
		October,
		November,
		December,
	};
	bool LeapYear(int year)
	{
		return (!(year % 4) && ((year % 100) || !(year % 400)));
	}
	int DaysInMonth(int month, int year)
	{
		switch (month)
		{
		case January:	return 31;
		case Febuary:	return (LeapYear(year) ? 29 : 28);
		case March:		return 31;
		case April:		return 30;
		case May:		return 31;
		case June:		return 30;
		case July:		return 31;
		case August:	return 31;
		case September: return 30;
		case October:	return 31;
		case November:	return 30;
		case December:	return 31;
		}
	}

	Date() : m_date() {}
	Date(size_t _date) : m_date(_date) {}

	size_t m_date;

	size_t Day()
	{
		return m_date;
	}
	size_t Week()
	{
		return m_date / 7;
	}
	size_t Month()
	{
		size_t month = 0;
		size_t date = m_date;
		size_t year = 0;
		while (date)
		{
			if (!(month % 12)) ++year;
			date -= DaysInMonth(month++, year);
		}
		return month - 1;
	}
	size_t Year()
	{

	}

	size_t DayOfWeek()
	{
		return m_date % 7;
	}
	size_t DayOfMonth()
	{
		return m_date - Month();
	}
	size_t DayOfYear()
	{

	}

	size_t WeekOfMonth()
	{

	}
	size_t WeekOfYear()
	{

	}

	size_t MonthOfYear()
	{

	}
	
};
struct Time
{
	Time() : {}

	unsigned char hour;
	unsigned char minute;
	unsigned char second;
};
struct FileMeta
{
	const char* name;
};
struct PNG
{

};
struct TXT
{
	
	const char* content;
};

int main()
{
	Date m_date = Date(12, 31, 2020);
	std::cout << std::bitset<8 * sizeof(m_date)>(*(unsigned int*)&m_date) << std::endl;
	printf("%u/%u/%#4u\n", m_date.day, m_date.month, m_date.year);
}