#include <stdio.h>			
#include <stdlib.h>
#include <assert.h>
#include <iostream>			
#include <fstream>			
#include <cstdarg>			
#include <string>			
#include <sstream>			
#include <unordered_map>	
#include <unordered_set>	
#include <list>				
#include <vector>			
#include <any>				

#define _DEPRECATE(_text) __declspec(deprecated(_text))
#define DEPRECATED(_replacement) _DEPRECATE("has been depricated. Please use '" #_replacement "' instead.")

#if _CONTAINER_DEBUG_LEVEL > 0
#define ASSERT(cond,message) _STL_VERIFY(cond,message);
#else
#define ASSERT(cond,message)
#endif // _CONTAINER_DEBUG_LEVEL > 0

using Tag = std::string;

// Proportional value (how much time do we want to spend on a tag compared to the others (when available))
std::unordered_map<Tag, float> g_Value
{
	{ "happiness",		001.0f },
	{ "fullfillment",	010.0f },
	{ "money",			100.0f },
	{ "education",		100.0f },
};

using Activity = std::string;

// Ratings for each tag for comparing activities
std::unordered_map<Activity, std::unordered_map<Tag, float>> g_ActivityTags
{
	{
		"drawing",
		{
			{ "happiness",		0.7f },
			{ "fullfillment",	0.5f },
			{ "money",			0.1f },
			{ "education",		0.0f },
		}
	},
	{
		"coding",
		{
			{ "happiness",		0.9f },
			{ "fullfillment",	0.8f },
			{ "money",			0.0f },
			{ "education",		0.1f },
		}
	},
	{
		"cellphone",
		{
			{ "happiness",		0.4f },
			{ "fullfillment",	0.0f },
			{ "money",			0.0f },
			{ "education",		0.0f },
		}
	},
	{
		"work",
		{
			{ "happiness",		0.0f },
			{ "fullfillment",	0.2f },
			{ "money",			1.0f },
			{ "education",		0.0f },
		}
	},
	{
		"school",
		{
			{ "happiness",		0.0f },
			{ "fullfillment",	0.0f },
			{ "money",			0.0f },
			{ "education",		1.0f },
		}
	},
};

using TimeChunkLabel = std::string;

// Labels for time chunks denoting what activities those chunks give access for
std::unordered_map<TimeChunkLabel, std::unordered_set<Activity>> g_Access
{
	{
		"freetime",
		{
			"drawing",
			"cellphone",
		}
	},
	{
		"office",
		{
			"coding",
			"drawing",
			"cellphone",
		}
	},
	{
		"work",
		{
			"work",
			"cellphone",
		}
	},
	{
		"school",
		{
			"school",
			"cellphone",
			"drawing",
		}
	},
};

enum class StatusHandling
{
	Norm, // Show as usual
	Hide, // Don't display (prerequisites unmet)
	Done, // Display crossed off or not at all (complete)
	Bump, // Show with importance
};

using Status = std::string;

// Allows the user to create custom task statuses
std::unordered_map<Status, StatusHandling> g_statusEffects
{
	{ "Complete",	StatusHandling::Done },
	{ "Waiting",	StatusHandling::Hide },
	{ "Critical",	StatusHandling::Bump },
	{ "Available",	StatusHandling::Norm },
};

struct Task
{
	std::string m_name;
	std::string m_desc;
	std::unordered_set<Task*> m_reliances;
	Status m_status;
};

// Tasks available to work on when possible
std::unordered_map<Activity, std::vector<Task>> g_tasklist
{
	{
		"coding",
		{
			{
				"Create a task",
				"Create/edit your tasks so you can start keeping track of your time!"
			},
			{
				"Finish the program",
				"it isn't done yet"
			},
		}
	},
};

// Things won't always start/end at exact times. This will allow for fuzziness on time chunks.
struct Range
{
	float min, max;
};

int main()
{
	printf("Name: ");
	std::string name;
	std::cin >> name;

	// Convert to all lower-case
	for (char& c : name)
	{
		if (c >= 'A' && c <= 'Z') // Capitals become lowercase
		{
			c = (c - 'A') + 'a';
		}
		if ((c < 'a' || c > 'z') && !(c == '-')) // No special symbols except hyphens
		{
			c = '0';
		}
	}
	for (int i = name.length() - 1; i >= 0; i--)
	{
		if (name[i] == '0') name.erase(i, 1);
	}

	printf("Nice to meet you, %s!", name.c_str());
	
	std::string filename = name + ".txt";

	FILE* file;
	fopen_s(&file, filename.c_str(), "r+");
	if (file)
	{
		// Todo: File exists

		fclose(file);
	}
	else
	{
		fopen_s(&file, filename.c_str(), "w+");
		if (file)
		{
			fprintf_s(file, name.c_str());

			fwrite();

			fclose(file);
		}
	}

	return 0;
}