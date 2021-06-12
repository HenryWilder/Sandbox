#pragma once
#include <map>
#include <unordered_map>
#include <vector>
#include <string>

struct Date;
struct Time;

struct Reservation;

struct Instructor;
using InstructorID_t = uint16_t;

struct Class;
using ClassID_t = uint32_t;

struct Customer;
using CustomerID_t = uint32_t;

enum class Membership : uint16_t;

enum class ReservationStatus : uint8_t;
enum class ReservationType : uint8_t;

namespace Intake
{
	extern std::map<CustomerID_t, Customer*>* g_customerMap;
	extern std::map<InstructorID_t, Instructor*>* g_instructorMap;
	extern std::map<ClassID_t, Class*>* g_classMap;
	extern Customer* g_this_customer;
	extern Reservation* g_this_reservation;
	extern Class* g_this_class;

	extern std::stringstream* g_line;

	extern std::string g_field;

	typedef void (*PullFunction)();
	extern std::vector<PullFunction> functionOrder_Customers;
	extern std::vector<PullFunction> functionOrder_Reservations;
	extern bool b_mode;


	void BeginCustomerMode();
	void BeginReservationMode();
	void InitMaps(std::map<CustomerID_t, Customer*>*		_customerMap,
				  std::map<InstructorID_t, Instructor*>*	_instructorMap,
				  std::map<ClassID_t, Class*>*				_classMap);
	void PushLine(std::stringstream* _line);
	
	void SetOrder(std::stringstream& headerLine);
	void AnalyzeLine();

#pragma region Helper Functions

	void GetField(std::string& dest);

	std::string& GetField();
	template<typename _Ty, _Ty(*_Callable)(const std::string&)> bool IfValid(_Ty& field);
	template<typename T> bool SetNumberFieldIfValid(T& field);
	bool SetDateFieldIfValid(Date& field);
	bool SetTimeFieldIfValid(Time& field);
	bool SetMembershipFieldIfValid(Membership& field);
	bool SetStatusFieldIfValid(ReservationStatus& field);
	bool SetPhoneFieldIfValid(uint64_t& field);

	void Skip();

#pragma endregion

#pragma region Pull Functions

	void PullCustomerID();
	void PullEmail();
	void PullName_Full();
	void PullPhone();
	void PullBirthday();

	void PullReservation_Type();
	void PullReservation_Status();
	void PullCreationDate();
	void PullCancelDate();
	void PullMembership();
	void PullClassID();
	void PullClassStartDate();
	void PullClassStartTime();
	void PullClassDuration();
	void PullInstructorID();
	void PullInstructorName();
	void PullHasSubstitute();
	void PushReservationToCustomer();

#pragma endregion
}