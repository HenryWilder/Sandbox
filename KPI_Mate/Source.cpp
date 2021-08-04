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
#include "Intake.h"

void ExportData(std::map<CustomerID_t, Customer*>* customerData, const char* file_and_path)
{
	std::ofstream file;
	file.open(file_and_path);
	file << "Name,Email,Phone,Birthday,Current Membership,Reservation Count,Class ID,Purchase Date,Purchase Membership,Status\n";
	for (auto[id, customer] : *customerData)
	{
		file << customer->name << ',' << customer->email << ',' << customer->phone << ',' << customer->birthday << ',' << customer->membership_current << ',' << customer->reservations.size();
		for (Reservation* reservation : customer->reservations)
		{
			file << "\n,,,,,," <<
				reservation->class_id << ',' <<
				reservation->purchase_date << ',' <<
				reservation->purchase_membership << ',' <<
				reservation->status ;
		}
		file << '\n';
	}

	file.close();
}

const char* g_debugArgs[3]
{
	"C:\\Users\\Admin\\Downloads\\CustomerData\\customers_all.csv",
	"C:\\Users\\Admin\\Downloads\\CustomerData\\reservations_all.csv",
	"C:\\Users\\Admin\\Downloads\\CustomerData\\analysis_all.csv"
};

int main(int argc, const char** argv)
{
	--argc;
	++argv;

	std::cout << "Args read: " << argc << std::endl;
	for (int i = 0; i < argc; ++i)
	{
		std::cout << argv[i] << std::endl;
	}
	if (argc < 3)
	{
		std::cout << "\nERROR: Missing command line arguments!\nSyntax:\n-path to customers csv\n-path to reservations csv\n-path to output\n-columns order\n";
		argv = new const char*[3];
		argv[0] = "C:\\Users\\Admin\\Downloads\\CustomerData\\customers_may.csv";
		argv[1] = "C:\\Users\\Admin\\Downloads\\CustomerData\\reservations_may.csv";
		argv[2] = "C:\\Users\\Admin\\Downloads\\CustomerData\\analysis_may.csv";
	}

	std::cout << "Running...\n";

	std::map<InstructorID_t, Instructor*> instructorsMap;
	std::map<ClassID_t, Class*> classesMap;
	std::map<CustomerID_t, Customer*> customersMap;

	std::ifstream customers;
	customers.open(argv[0]);
	std::ifstream reservations;
	reservations.open(argv[1]);

	Intake::InitMaps(&customersMap, &instructorsMap, &classesMap);

	std::string str;
	Intake::BeginCustomerMode();
	{
		std::getline(customers, str, '\n');
		std::stringstream header(str);
		Intake::SetOrder(header);
	}
	while (std::getline(customers, str, '\n'))
	{
		std::stringstream line(str);
		Intake::PushLine(&line);
		Intake::AnalyzeLine();
		std::cout << 'C';
	}
	Intake::BeginReservationMode();
	{
		std::getline(reservations, str, '\n');
		std::stringstream header(str);
		Intake::SetOrder(header);
	}
	while (std::getline(reservations, str, '\n'))
	{
		std::stringstream line(str);
		Intake::PushLine(&line);
		Intake::AnalyzeLine();
		std::cout << 'R';
	}

	reservations.close();
	customers.close();

	ExportData(&customersMap, argv[2]);

	std::cout << "\nFinished.\n";
	std::cin.ignore();
	return 0;
}
