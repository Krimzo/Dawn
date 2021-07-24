#pragma once
#include <vector>
#include <fstream>
#include "MyStuff/MyStruct.h"


std::vector<std::string> GetCommands(std::string stringOfCommands);
void HandleCommand(std::string command, int commandIndex);

