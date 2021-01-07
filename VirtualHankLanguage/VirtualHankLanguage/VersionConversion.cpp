#include "VersionConversion.h"

std::string FormatText(const char* text, ...)
{
	char buffer[1024] = {};
	va_list args;
	va_start(args, text);
	vsprintf_s(buffer, text, args);
	va_end(args);
	return buffer;
}

std::ifstream& operator>>(std::ifstream& stream, Change change)
{
	stream >> change.from >> change.to;
	return stream;
}

void UpdateFile(const char* filename, int fVersion)
{
	std::ifstream versionFormatFile = std::ifstream(FormatText("%i.vhvf", fVersion).c_str());
	printf("Using \"%s\" to update the file \"%s\"\n", FormatText("%i.vhvf", fVersion).c_str(), filename);
	std::vector<Change> changelog;
	while (true) {
		Change change;
		versionFormatFile >> change;
		changelog.push_back(change);
	}

	versionFormatFile.close();
}
