#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

struct Date
{
	uint16_t year, month, day;
	bool IsNull();
};
Date DateZero();
bool operator<(Date a, Date b);
bool operator<=(Date a, Date b);
bool operator>(Date a, Date b);
bool operator>=(Date a, Date b);
bool operator==(Date a, Date b);
bool operator!=(Date a, Date b);
Date operator-(Date a, Date b);
Date operator+(Date a, Date b);
Date DateFromString(const std::string& str);

struct Time
{
	uint16_t hour, minute, second;
	bool IsNull();
};
Time TimeZero();
bool operator<(Time a, Time b);
bool operator<=(Time a, Time b);
bool operator>(Time a, Time b);
bool operator>=(Time a, Time b);
bool operator==(Time a, Time b);
bool operator!=(Time a, Time b);
Time operator-(Time a, Time b);
Time operator+(Time a, Time b);
Time TimeFromString(const std::string& str);

enum class Membership : uint16_t
{
	None,
	W2Trial,
	Y1Unlim,
	Y1Unlim1,
	Y1Unlim2,
	Y1Unlim3,
	M1Unlim,
	M3Unlim,
	M3Unlim1,
	M3UnlimT,
	WUnlimSpec,
	WUnlim,
	WUnlim12WMin,
	C1Pack,
	C2Pack,
	C3Pack,
	C4Pack,
	C5Pack,
	C10Pack,
	Livestream,
	LunarLive,
	Mini,
	Trainee,
	Employee,
	PromoUnlim,
	OtherNotVoid,
};
Membership MembershipFromString(const std::string& str);
std::string MembershipToString(Membership membership);
uint64_t PhoneFromString(const std::string& str);

std::istream& operator>>(std::istream& stream, Membership& membership);
std::ostream& operator<<(std::ostream& stream, Membership membership);

struct Instructor;
using InstructorID_t = uint16_t;

struct Class;
using ClassID_t = uint32_t;

struct Customer;
using CustomerID_t = uint32_t;

struct Instructor
{
	std::string name = "Name Not Found";
	std::vector<ClassID_t> classes;
};

enum class ClassType : uint8_t
{
	MOONBEATS,
	CYCLE_HIIT,
	CYCLE_HYPNOS,
};
struct Class
{
	ClassType type = ClassType::MOONBEATS;
	bool substitute = false;
	InstructorID_t instructor_id = 0;
	Date class_date = DateZero();
	Time class_time = TimeZero();
	unsigned short duration = 0; // In minutes
};

enum class ReservationStatus : uint8_t
{
	check_in,
	no_show_graced,
	no_show_penalty,
	cancel_standard,
	cancel_graced,
	cancel_penalty,
	cancel_class,
	removed,
};
enum class ReservationType : uint8_t
{
	standard,
	waitlist
};
struct Reservation
{
	ClassID_t class_id = 0;
	Date purchase_date = DateZero();
	Date cancel_date = DateZero(); // Only if cancelled
	unsigned short spot = 0;
	Membership purchase_membership = Membership::None;
	ReservationStatus status;
	ReservationType type;
};
ReservationStatus StatusFromString(const std::string& str);
std::string StatusToString(ReservationStatus status);
std::ostream& operator<<(std::ostream& stream, ReservationStatus status);

struct Customer
{
	std::string name = "Name not found";
	std::string email = "Email not found";
	uint64_t phone = 0;
	Date birthday = DateZero();
	Membership membership_current = Membership::None;
	std::vector<Reservation*> reservations;
};

std::ostream& operator<<(std::ostream& stream, Date date);
std::ostream& operator<<(std::ostream& stream, Time time);