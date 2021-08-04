#include "Intake.h"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <unordered_map>
#include <stdio.h>
#include <functional>
#include <algorithm>
#include <set>
#include <unordered_set>
#include <type_traits>

#include "Data.h"

namespace Intake
{
	std::map<CustomerID_t, Customer*>* g_customerMap = nullptr;
	std::map<InstructorID_t, Instructor*>* g_instructorMap = nullptr;
	std::map<ClassID_t, Class*>* g_classMap = nullptr;
	Customer* g_this_customer = nullptr;
	Reservation* g_this_reservation = nullptr;
	Class* g_this_class = nullptr;

	std::stringstream* g_line = nullptr;

	std::string g_field;

	std::vector<PullFunction> functionOrder_Customers;
	std::vector<PullFunction> functionOrder_Reservations;
	bool b_mode;

	void BeginCustomerMode()
	{
		std::cout << "\nReading Customers...\n";
		b_mode = 0;
	}
	void BeginReservationMode()
	{
		std::cout << "\nReading Reservations...\n";
		b_mode = 1;
	}
	void InitMaps(
		std::map<CustomerID_t, Customer*>* _customerMap,
		std::map<InstructorID_t, Instructor*>* _instructorMap,
		std::map<ClassID_t, Class*>* _classMap)
	{
		g_customerMap = _customerMap;
		g_instructorMap = _instructorMap;
		g_classMap = _classMap;
	}
	void PushLine(std::stringstream* _line)
	{
		g_line = _line;
	}

	void SetOrder(std::stringstream& headerLine)
	{
		static const std::unordered_map<std::string, PullFunction> s_functionMap =
		{
			{ "Customer ID",				PullCustomerID				},
			{ "Customer Email",				PullEmail					},
			{ "Email",						PullEmail					},
			{ "Customer Name",				PullName_Full				},
			{ "Full Name",					PullName_Full				},
			{ "Phone Number",				PullPhone					},
			{ "Birth Date",					PullBirthday				},
			{ "Reservation Type",			PullReservation_Type		},
			{ "Status",						PullReservation_Status		},
			{ "Creation Date",				PullCreationDate			},
			{ "Cancelled Date",				PullCancelDate				},
			{ "Package",					PullMembership				},
			{ "Contract",					PullMembership				},
			{ "Class ID",					PullClassID					},
			{ "Class Start Date",			PullClassStartDate			},
			{ "Class Start Time",			PullClassStartTime			},
			{ "Class Duration (Minutes)",	PullClassDuration			},
			{ "Instructor ID(s)",			PullInstructorID			},
			{ "Instructor Names",			PullInstructorName			},
			{ "Class Has Substitute?",		PullHasSubstitute			},
			{ "Assoc_Reserv",				PushReservationToCustomer	},
		};
		std::string field;
		std::vector<PullFunction>& orderVec = (b_mode ? functionOrder_Reservations : functionOrder_Customers);
		while (std::getline(headerLine, field, ','))
		{
			auto it = s_functionMap.find(field);
			if (it != s_functionMap.end())
				orderVec.push_back(it->second);
			else
				orderVec.push_back(Skip);
		}
	}
	void AnalyzeLine()
	{
#if _DEBUG
		char column0 = 'A';
		char column1 = 'A' - 1;
#endif
		for (PullFunction f : (b_mode ? functionOrder_Reservations : functionOrder_Customers))
		{
#if _DEBUG
			std::cout << (column1 >= 'A' ? column1 : ' ') << column0 << ' ';
#endif
			f();
#if _DEBUG
			++column0;
			if (column0 > 'Z')
			{
				column0 = 'A';
				++column1;
			}
#endif
		}
		if (b_mode)
			PushReservationToCustomer();
#if _DEBUG
		std::cout << std::endl;
#endif
	}

#define ENFORCE_INIT_VAR(var) _ASSERT_EXPR(var != nullptr, "ERROR! Attempted to store to variable when variable was not set!")
#define CREATE_IF_UNINIT(var) if (var == nullptr) { var = new std::remove_pointer_t<decltype(var)>; std::cout << 'n'; } else std::cout << ""

#pragma region Helper Functions

	void GetField(std::string& dest)
	{
		ENFORCE_INIT_VAR(g_line);
		if (g_line->peek() == '\"')
		{
			g_line->ignore();
			std::getline(*g_line, dest, '\"');
			size_t count = dest.find(',');
			if (count != dest.npos)
				dest = dest.substr(0, count);
			g_line->ignore();
		}
		else
		{
			std::getline(*g_line, dest, ',');
		}

#if _DEBUG
		std::cout << dest << ' ';
#endif
	}

	std::string& GetField()
	{
		GetField(g_field);
		return g_field;
	}
	template<typename _Ty, _Ty(*_Callable)(const std::string&)>
	bool IfValid(_Ty& field)
	{
		GetField();
		bool b = !g_field.empty();
		if (b)
			field = _Callable(g_field);
		return b;
	}
	template<typename T>
	T stot(const std::string& str)
	{
		return static_cast<T>(std::stoll(str));
	}
	template<typename T>
	bool SetNumberFieldIfValid(T& field)
	{
		return IfValid<T, stot<T>>(field);
	}
	bool SetDateFieldIfValid(Date& field)
	{
		return IfValid<Date, DateFromString>(field);
	}
	bool SetTimeFieldIfValid(Time& field)
	{
		return IfValid<Time, TimeFromString>(field);
	}
	bool SetMembershipFieldIfValid(Membership& field)
	{
		return IfValid<Membership, MembershipFromString>(field);
	}
	bool SetStatusFieldIfValid(ReservationStatus& field)
	{
		return IfValid<ReservationStatus, StatusFromString>(field);
	}
	bool SetPhoneFieldIfValid(uint64_t& field)
	{
		return IfValid<uint64_t, PhoneFromString>(field);
	}

	void Skip()
	{
		ENFORCE_INIT_VAR(g_line);
#if _DEBUG
		std::string str;
		if (g_line->peek() == '\"')
		{
			g_line->ignore();
			getline(*g_line, str, '\"');
			g_line->ignore();
		}
		else
			getline(*g_line, str, ',');
		std::cout << '<' << str << ">  ";
#else
		if (g_line->peek() == '\"')
		{
			g_line->ignore();
			g_line->ignore(128, '\"');
			g_line->ignore();
		}
		else
			g_line->ignore(128, ',');
#endif
	}

#pragma endregion
#if _DEBUG
#define DEBUG_NAME std::cout << __func__ << ": "
#else
#define DEBUG_NAME
#endif

	void PullCustomerID()
	{
		DEBUG_NAME;

		CustomerID_t id;
		if (!SetNumberFieldIfValid(id))
			id = 0;

		auto it = g_customerMap->find(id);
		if (it != g_customerMap->end())
		{
			g_this_customer = it->second;
		}
		else
		{
			g_this_customer = new Customer;
			g_customerMap->insert({ id, g_this_customer });
		}
	}
	void PullEmail()
	{
		DEBUG_NAME;
		ENFORCE_INIT_VAR(g_this_customer);
		GetField(g_this_customer->email);
	}
	void PullName_Full()
	{
		DEBUG_NAME;
		ENFORCE_INIT_VAR(g_this_customer);
		GetField(g_this_customer->name);
	}
	void PullPhone()
	{
		DEBUG_NAME;
		ENFORCE_INIT_VAR(g_this_customer);
		SetPhoneFieldIfValid(g_this_customer->phone);
	}
	void PullBirthday()
	{
		DEBUG_NAME;
		ENFORCE_INIT_VAR(g_this_customer);
		SetDateFieldIfValid(g_this_customer->birthday);
	}

	void PullReservation_Type()
	{
		DEBUG_NAME;
		CREATE_IF_UNINIT(g_this_reservation);
		GetField();
		if (g_field == "standard")
			g_this_reservation->type = ReservationType::standard;
		else if (g_field == "waitlist")
			g_this_reservation->type = ReservationType::waitlist;
	}
	void PullReservation_Status()
	{
		DEBUG_NAME;
		CREATE_IF_UNINIT(g_this_reservation);
		g_this_reservation->status = StatusFromString(GetField());
	}
	void PullCreationDate()
	{
		DEBUG_NAME;
		CREATE_IF_UNINIT(g_this_reservation);
		g_this_reservation->purchase_date = DateFromString(GetField());
	}
	void PullCancelDate()
	{
		DEBUG_NAME;
		CREATE_IF_UNINIT(g_this_reservation);
		if (g_this_reservation->status == ReservationStatus::cancel_standard ||
			g_this_reservation->status == ReservationStatus::cancel_graced ||
			g_this_reservation->status == ReservationStatus::cancel_penalty)
		{
			g_this_reservation->cancel_date = DateFromString(GetField());
		}
		else
		{
			g_this_reservation->cancel_date = DateZero();
			Skip();
		}
	}
	void PullMembership()
	{
		DEBUG_NAME;
		CREATE_IF_UNINIT(g_this_reservation);
		if (g_this_reservation->purchase_membership == Membership::None)
			SetMembershipFieldIfValid(g_this_reservation->purchase_membership);
		else
			Skip();
	}
	void PullClassID()
	{
		DEBUG_NAME;
		CREATE_IF_UNINIT(g_this_reservation);
		ENFORCE_INIT_VAR(g_classMap);
		g_this_reservation->class_id = std::stoul(GetField());
		{
			auto it = g_classMap->find(g_this_reservation->class_id);
			if (it != g_classMap->end())
				g_this_class = it->second;
			else
				g_this_class = new Class;
		}
	}
	void PullClassStartDate()
	{
		DEBUG_NAME;
		ENFORCE_INIT_VAR(g_this_class);
		g_this_class->class_date = DateFromString(GetField());
	}
	void PullClassStartTime()
	{
		DEBUG_NAME;
		ENFORCE_INIT_VAR(g_this_class);
		g_this_class->class_time = TimeFromString(GetField());
	}
	void PullClassDuration()
	{
		DEBUG_NAME;
		ENFORCE_INIT_VAR(g_this_class);
		g_this_class->duration = (unsigned short)std::stoi(GetField());
	}
	void PullInstructorID()
	{
		DEBUG_NAME;
		ENFORCE_INIT_VAR(g_this_class);
		SetNumberFieldIfValid(g_this_class->instructor_id);
	}
	void PullInstructorName()
	{
		DEBUG_NAME;
		ENFORCE_INIT_VAR(g_instructorMap);
		ENFORCE_INIT_VAR(g_this_class);
		auto it = g_instructorMap->find(g_this_class->instructor_id);
		if (it == g_instructorMap->end())
		{
			g_instructorMap->insert({ g_this_class->instructor_id, new Instructor });
			it = g_instructorMap->find(g_this_class->instructor_id);
			GetField(it->second->name);
		}
		else
		{
			Skip();
		}
		it->second->classes.push_back(g_this_reservation->class_id);
	}
	void PullHasSubstitute()
	{
		DEBUG_NAME;
		ENFORCE_INIT_VAR(g_this_class);
		GetField();
		for (char& c : g_field)
		{
			if (c >= 'A' && c <= 'Z')
				c = (c - 'A') + 'a';
		}
		if (g_field == "true")
			g_this_class->substitute = true;
		else
			g_this_class->substitute = false;
	}
	void PushReservationToCustomer()
	{
		DEBUG_NAME;
		ENFORCE_INIT_VAR(g_this_customer);
		CREATE_IF_UNINIT(g_this_reservation);
		g_this_customer->reservations.push_back(g_this_reservation);
		g_this_reservation = nullptr;
	}

#undef DEBUG_NAME
#pragma endregion
#undef ENFORCE_INIT_VAR
#undef CREATE_IF_UNINIT
}
