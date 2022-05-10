#include "Utility/Utility.h"


size_t Utility::ReplaceInString(String& str, const String& from, const String& to) {
	size_t foundAt = 0;
	size_t replaceCounter = 0;
	while ((foundAt = str.find(from, foundAt)) != String::npos) {
		str.replace(foundAt, from.length(), to);
		foundAt += to.length();
		replaceCounter++;
	}
	return replaceCounter;
}

void Utility::ClearSource(std::stringstream& src) {
	String srcData = src.str();
	while (ReplaceInString(srcData, "\t", " "));
	while (ReplaceInString(srcData, "  ", " "));
	while (ReplaceInString(srcData, "\n ", "\n"));
	if (srcData.front() == ' ') srcData.erase(srcData.begin());
	src = std::stringstream(srcData);
}
