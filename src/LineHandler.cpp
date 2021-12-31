#include "LineHandler.h"

#include <iostream>
#include <string>

#include "MiscFuncs.h"


// Handles the source line 
void HandleLine(std::string& line) {
	static int n = 0;
	std::cout << "Line " << n++ << ": " << line << std::endl;
}
