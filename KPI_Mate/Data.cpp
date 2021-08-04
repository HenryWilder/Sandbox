#include "Data.h"
#include <string>
#include <iostream>
#include <sstream>

bool Date::IsNull()
{
	return !(year || month || day);
}
Date DateZero()
{
	static constexpr Date d0{ 0,0,0 };
	return d0;
}
bool operator<(Date a, Date b)
{
	if (a.year < b.year)
		return true;
	else if (a.year > b.year)
		return false;
	else
	{
		if (a.month < b.month)
			return true;
		else if (a.month > b.month)
			return false;
		else
		{
			if (a.day < b.day)
				return true;
			else
				return false;
		}
	}
}
bool operator<=(Date a, Date b)
{
	if (a.year < b.year)
		return true;
	else if (a.year > b.year)
		return false;
	else
	{
		if (a.month < b.month)
			return true;
		else if (a.month > b.month)
			return false;
		else
		{
			if (a.day <= b.day)
				return true;
			else
				return false;
		}
	}
}
bool operator>(Date a, Date b)
{
	if (a.year > b.year)
		return true;
	else if (a.year < b.year)
		return false;
	else
	{
		if (a.month > b.month)
			return true;
		else if (a.month < b.month)
			return false;
		else
		{
			if (a.day > b.day)
				return true;
			else
				return false;
		}
	}
}
bool operator>=(Date a, Date b)
{
	if (a.year > b.year)
		return true;
	else if (a.year < b.year)
		return false;
	else
	{
		if (a.month > b.month)
			return true;
		else if (a.month < b.month)
			return false;
		else
		{
			if (a.day >= b.day)
				return true;
			else
				return false;
		}
	}
}
bool operator==(Date a, Date b)
{
	return
		a.year == b.year &&
		a.month == b.month &&
		a.day == b.day;
}
bool operator!=(Date a, Date b)
{
	return
		a.year != b.year ||
		a.month != b.month ||
		a.day != b.day;
}
Date operator-(Date a, Date b)
{
	Date ret;
	ret.year = a.year - b.year;
	ret.month = a.month - b.month;
	ret.day = a.day - b.day;
	return ret;
}
Date operator+(Date a, Date b)
{
	Date ret;
	ret.year = a.year + b.year;
	ret.month = a.month + b.month;
	ret.day = a.day + b.day;
	return ret;
}
Date DateFromString(const std::string& str)
{
	if (str.empty())
		return Date{ 0,0,0 };

	Date ret;
	std::stringstream date(str);
	char divider;

	if (str.find_first_not_of("0123456789") == 4)
	{
		date >> ret.year >> divider >> ret.month >> divider >> ret.day;
	}
	else
	{
		date >> ret.month >> divider >> ret.day >> divider >> ret.year;
	}
	return ret;
}

bool Time::IsNull()
{
	return !(hour || minute || second);
}
Time TimeZero()
{
	static constexpr Time t0{ 0,0,0 };
	return t0;
}
bool operator<(Time a, Time b)
{
	if (a.hour < b.hour)
		return true;
	else if (a.hour > b.hour)
		return false;
	else
	{
		if (a.minute < b.minute)
			return true;
		else if (a.minute > b.minute)
			return false;
		else
		{
			if (a.second < b.second)
				return true;
			else
				return false;
		}
	}
}
bool operator<=(Time a, Time b)
{
	if (a.hour < b.hour)
		return true;
	else if (a.hour > b.hour)
		return false;
	else
	{
		if (a.minute < b.minute)
			return true;
		else if (a.minute > b.minute)
			return false;
		else
		{
			if (a.second <= b.second)
				return true;
			else
				return false;
		}
	}
}
bool operator>(Time a, Time b)
{
	if (a.hour > b.hour)
		return true;
	else if (a.hour < b.hour)
		return false;
	else
	{
		if (a.minute > b.minute)
			return true;
		else if (a.minute < b.minute)
			return false;
		else
		{
			if (a.second > b.second)
				return true;
			else
				return false;
		}
	}
}
bool operator>=(Time a, Time b)
{
	if (a.hour > b.hour)
		return true;
	else if (a.hour < b.hour)
		return false;
	else
	{
		if (a.minute > b.minute)
			return true;
		else if (a.minute < b.minute)
			return false;
		else
		{
			if (a.second >= b.second)
				return true;
			else
				return false;
		}
	}
}
bool operator==(Time a, Time b)
{
	return
		a.hour == b.hour &&
		a.minute == b.minute &&
		a.second == b.second;
}
bool operator!=(Time a, Time b)
{
	return
		a.hour != b.hour ||
		a.minute != b.minute ||
		a.second != b.second;
}
Time operator-(Time a, Time b)
{
	Time ret;
	ret.hour = a.hour - b.hour;
	ret.minute = a.minute - b.minute;
	ret.second = a.second - b.second;
	return ret;
}
Time operator+(Time a, Time b)
{
	Time ret;
	ret.hour = a.hour + b.hour;
	ret.minute = a.minute + b.minute;
	ret.second = a.second + b.second;
	return ret;
}
Time TimeFromString(const std::string& str)
{
	Time ret;
	ret.hour = std::stoi(str.substr(0, 2));
	ret.minute = std::stoi(str.substr(3, 2));
	ret.second = std::stoi(str.substr(6, 2));
	return ret;
}

Membership MembershipFromString(const std::string& str)
{
	static const std::unordered_map<std::string, Membership> s_MembershipHash =
	{
		{ "2 WEEK TRIAL",									Membership::W2Trial			},
		{ "1 YEAR UNLIMITED",								Membership::Y1Unlim			},
		{ "1 YEAR UNLIMITED I",								Membership::Y1Unlim1		},
		{ "1 YEAR UNLIMITED II",							Membership::Y1Unlim2		},
		{ "1 YEAR UNLIMITED III",							Membership::Y1Unlim3		},
		{ "1 MONTH UNLIMITED",								Membership::M1Unlim			},
		{ "3 MONTH UNLIMITED",								Membership::M3Unlim			},
		{ "3 MONTH UNLIMITED I",							Membership::M3Unlim1		},
		{ "TRIBE - 3 MONTH UNLIMITED",						Membership::M3UnlimT		},
		{ "SPECIAL - WEEKLY UNLIMITED",						Membership::WUnlimSpec		},
		{ "WEEKLY UNLIMITED",								Membership::WUnlim			},
		{ "WEEKLY UNLIMITED - 12 WEEK MIN",					Membership::WUnlim12WMin	},
		{ "1 CLASS",										Membership::C1Pack			},
		{ "1 CLASS PACK",									Membership::C1Pack			},
		{ "2 CLASS",										Membership::C2Pack			},
		{ "2 CLASS PACK",									Membership::C2Pack			},
		{ "3 CLASS",										Membership::C3Pack			},
		{ "3 CLASS PACK",									Membership::C3Pack			},
		{ "4 CLASS",										Membership::C4Pack			},
		{ "4 CLASS PACK",									Membership::C4Pack			},
		{ "5 CLASS",										Membership::C5Pack			},
		{ "5 CLASS PACK",									Membership::C5Pack			},
		{ "10 CLASS PACK",									Membership::C10Pack			},
		{ "10 CLASS",										Membership::C10Pack			},
		{ "LIVESTREAM MEMBERSHIP",							Membership::Livestream		},
		{ "LUNAR LIVE! AND BIKE RENTAL - MOON LANDER",		Membership::LunarLive		},
		{ "MINI MEMBERSHIP",								Membership::Mini			},
		{ "TRAINEE - UNLIMITED MEMBERSHIP",					Membership::Trainee			},
		{ "TRADE - EMPLOYEE MEMBERSHIP",					Membership::Employee		},
		{ "TRADE - PROMO UNLIMITED",						Membership::PromoUnlim		},
	};

	if (str.empty())
		return Membership::None;

	auto it = s_MembershipHash.find(str);

	if (it == s_MembershipHash.end())
	{
		std::cout << "{ " << str << " }";
		return Membership::OtherNotVoid;
	}
	else
		return it->second;
}
std::string MembershipToString(Membership membership)
{
	switch (membership)
	{
	default:							return "ERR: Unset in MembershipToString function";
	case Membership::None:				return "";
	case Membership::W2Trial:			return "2 WEEK TRIAL";
	case Membership::Y1Unlim:			return "1 YEAR UNLIMITED";
	case Membership::Y1Unlim1:			return "1 YEAR UNLIMITED I";
	case Membership::Y1Unlim2:			return "1 YEAR UNLIMITED II";
	case Membership::Y1Unlim3:			return "1 YEAR UNLIMITED III";
	case Membership::M1Unlim:			return "1 MONTH UNLIMITED";
	case Membership::M3Unlim:			return "3 MONTH UNLIMITED";
	case Membership::M3Unlim1:			return "3 MONTH UNLIMITED I";
	case Membership::M3UnlimT:			return "TRIBE - 3 MONTH UNLIMITED";
	case Membership::WUnlimSpec:		return "SPECIAL - WEEKLY UNLIMITED";
	case Membership::WUnlim:			return "WEEKLY UNLIMITED";
	case Membership::WUnlim12WMin:		return "WEEKLY UNLIMITED - 12 WEEK MIN";
	case Membership::C1Pack:			return "1 CLASS PACK";
	case Membership::C2Pack:			return "2 CLASS PACK";
	case Membership::C3Pack:			return "3 CLASS PACK";
	case Membership::C4Pack:			return "4 CLASS PACK";
	case Membership::C5Pack:			return "5 CLASS PACK";
	case Membership::C10Pack:			return "10 CLASS PACK";
	case Membership::Livestream:		return "LIVESTREAM MEMBERSHIP";
	case Membership::LunarLive:			return "LUNAR LIVE! AND BIKE RENTAL - MOON LANDER";
	case Membership::Mini:				return "MINI MEMBERSHIP";
	case Membership::Trainee:			return "TRAINEE - UNLIMITED MEMBERSHIP";
	case Membership::Employee:			return "EMPLOYEE MEMBERSHIP";
	case Membership::PromoUnlim:		return "TRADE - PROMO UNLIMITED";
	}
}
uint64_t PhoneFromString(const std::string& str)
{
	if (str.empty())
		return 0;

	size_t openParen = str.find('(');
	size_t closeParen = str.find(')');
	size_t space = str.find(' ');
	size_t dash0 = str.find('-');
	size_t dash1 = str.find('-', dash0 + 1);
	size_t dot0 = str.find('.');
	size_t dot1 = str.find('.', dot0 + 1);

	if (str.find_first_of("() -.") != str.npos)
	{
		std::string str2 = str;
		while (true)
		{
			size_t pos = str2.find_last_of("() -.");
			size_t count = 1;

			if (pos == str.npos)
				break;

			str2.erase(pos, count);
		}

		return std::stoull(str2);
	}

	return std::stoull(str);
}

std::istream& operator>>(std::istream& stream, Membership& membership)
{
	std::string str;
	if (std::getline(stream, str, ','))
		membership = MembershipFromString(str);
	else
		membership = Membership::None;
	return stream;
}
std::ostream& operator<<(std::ostream& stream, Membership membership)
{
	stream << MembershipToString(membership);
	return stream;
}

ReservationStatus StatusFromString(const std::string& str)
{
	static const std::unordered_map<std::string, ReservationStatus> s_statusMap =
	{
		{ "check in",			ReservationStatus::check_in				},
		{ "graced no show",		ReservationStatus::no_show_graced		},
		{ "penalty no show",	ReservationStatus::no_show_penalty		},
		{ "standard cancel",	ReservationStatus::cancel_standard		},
		{ "graced cancel",		ReservationStatus::cancel_graced		},
		{ "penalty cancel",		ReservationStatus::cancel_penalty		},
		{ "class cancelled",	ReservationStatus::cancel_class			},
		{ "removed",			ReservationStatus::removed				},
	};
	auto it = s_statusMap.find(str);
	_ASSERT_EXPR(it != s_statusMap.end(), "Status could not be recognized!");
	return it->second;
}
std::string StatusToString(ReservationStatus status)
{
	switch (status)
	{
	case ReservationStatus::check_in:			return "check in";
	case ReservationStatus::no_show_graced:		return "graced no show";
	case ReservationStatus::no_show_penalty:	return "penalty no show";
	case ReservationStatus::cancel_standard:	return "standard cancel";
	case ReservationStatus::cancel_graced:		return "graced cancel";
	case ReservationStatus::cancel_penalty:		return "graced penalty";
	case ReservationStatus::cancel_class:		return "class cancelled";
	case ReservationStatus::removed:			return "removed";
	default: return "unknown";
	}
}
std::ostream& operator<<(std::ostream& stream, ReservationStatus status)
{
	return stream << StatusToString(status);
}

std::ostream& operator<<(std::ostream& stream, Date date)
{
	if (date == DateZero())
		return stream;

	if (date.month < 10)
		stream << '0';
	stream << date.month << "/";
	if (date.day < 10)
		stream << '0';
	stream << date.day << "/" << date.year;
	return stream;
}
std::ostream& operator<<(std::ostream& stream, Time time)
{
	if (time == TimeZero())
		return stream;

	if (time.hour < 10)
		stream << '0';
	stream << time.hour << ":";
	if (time.minute < 10)
		stream << '0';
	stream << time.minute << ":";
	if (time.second < 10)
		stream << '0';
	stream << time.second;
	return stream;
}
