#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include "MyStuff/MyGlobal.h"
#include "MyStuff/MyStruct.h"
#include "MyStuff/MyFunc.h"


std::vector<std::string> GetCommands(std::string stringOfCommands) {
    std::vector<std::string> commands;

    std::string tempString;
    int bracketCounter = 0;
    int lastFoundClosedBracket = 0;
    int newLineCount = 0;
    bool commentFound = false;
    for (int i = 0; i < stringOfCommands.length(); i++) {
        if (!commentFound) {
            switch (stringOfCommands[i]) {
            case 9:
                break;
            case '\n':
                if (bracketCounter == 0) {
                    if (tempString != "") {
                        while (tempString.find("  ") != -1) {
                            tempString.replace(tempString.find("  "), 2, " ");
                        }
                        if (tempString != " ") {
                            if (tempString[0] == ' ') {
                                tempString.erase(0, 1);
                            }
                            commands.push_back(tempString);
                        }
                        tempString.clear();
                    }
                }
                else {
                    tempString.push_back('~');
                }
                newLineCount++;
                break;
            case '{':
                tempString.push_back(stringOfCommands[i]);
                bracketCounter++;
                lastFoundClosedBracket = newLineCount + 1;
                break;
            case '}':
                tempString.push_back(stringOfCommands[i]);
                bracketCounter--;
                if (bracketCounter == 0) {
                    while (tempString.find("~~") != -1) {
                        tempString.replace(tempString.find("~~"), 2, "~");
                    }
                    while (tempString.find(" ~") != -1) {
                        tempString.replace(tempString.find(" ~"), 2, "~");
                    }
                    while (tempString.find("{~") != -1) {
                        tempString.replace(tempString.find("{~"), 2, "{");
                    }
                    while (tempString.find("}~") != -1) {
                        tempString.replace(tempString.find("}~"), 2, "}");
                    }
                    while (tempString.find("  ") != -1) {
                        tempString.replace(tempString.find("  "), 2, " ");
                    }
                    if (tempString[0] == ' ') {
                        tempString.erase(0, 1);
                    }
                    commands.push_back(tempString);
                    tempString.clear();
                }
                else if (bracketCounter < 0) {
                    printf("Excess '}' found, at line %d\n", newLineCount + 1);
                    exit(1);
                }
                break;
            case ';':
                printf("FaxDown doesn't use ';' found, at line %d\n", newLineCount + 1);
                exit(1);
                break;
            case '$':
                if (bracketCounter == 0) {
                    if (tempString != "") {
                        while (tempString.find("  ") != -1) {
                            tempString.replace(tempString.find("  "), 2, " ");
                        }
                        if (tempString != " ") {
                            if (tempString[0] == ' ') {
                                tempString.erase(0, 1);
                            }
                            commands.push_back(tempString);
                        }
                        tempString.clear();
                    }
                }
                commentFound = true;
                break;
            default:
                tempString.push_back(stringOfCommands[i]);
                break;
            }
        }
        else if (stringOfCommands[i] == '\n') {
            commentFound = false;
            newLineCount++;
        }
    }

    if (bracketCounter > 0) {
        printf("Excess '{' found, at line %d\n", lastFoundClosedBracket);
        exit(1);
    }

    return commands;
}


void HandleCommand(std::string command, int commandIndex) {
    if (command.find("(") == -1) {
        FaxDownVar tempVar = { "", false, 0, 0.0, "" };

        if (command.back() != ' ') {
            command.push_back(' ');
        }

        std::string tempString;
        int actionType = 0;
        int dataType = 0;
        for (int i = 0; i < command.length(); i++) {
            if ((command[i] != ' ' || actionType == 3) && i != command.length() - 1) {
                tempString.push_back(command[i]);
            }
            else {
                // Var type
                if (actionType == 0) {
                    if (tempString == "bool") {
                        dataType = 0;
                    }
                    else if (tempString == "int") {
                        dataType = 1;
                    }
                    else if (tempString == "float") {
                        dataType = 2;
                    }
                    else if (tempString == "string") {
                        dataType = 3;
                    }
                    else {
                        printf("Bad data type, at command %d\n", commandIndex);
                        exit(1);
                    }
                    tempString.clear();
                    actionType = 1;
                }
                // Var name
                else if (actionType == 1) {
                    for (int j = 0; j < tempString.length(); j++) {
                        if (j == 0) {
                            if (!((tempString[j] > 64 && tempString[j] < 91) || (tempString[j] > 96 && tempString[j] < 123))) {
                                printf("First char of the variable name can't be a number, at command %d\n", commandIndex);
                                exit(1);
                            }
                        }
                        else {
                            if (!((tempString[j] > 64 && tempString[j] < 91) || (tempString[j] > 96 && tempString[j] < 123) || (tempString[j] > 47 && tempString[j] < 58))) {
                                printf("Bad name assigned, at command %d\n", commandIndex);
                                exit(1);
                            }
                        }
                    }
                    tempVar.varName = tempString;
                    tempString.clear();
                    actionType = 2;
                }
                // =
                else if (actionType == 2) {
                    if (tempString != "=") {
                        printf("Bad variable assignment, at command %d\n", commandIndex);
                        exit(1);
                    }
                    tempString.clear();
                    actionType = 3;
                }
                // Var value
                else if (actionType == 3) {
                    if (dataType == 0) {
                        if (tempString == "true") {
                            tempVar.boolValue = true;
                        }
                        else if (tempString == "false") {
                            tempVar.boolValue = false;
                        }
                        else {
                            printf("Bad data assignment, at command %d\n", commandIndex);
                            exit(1);
                        }
                    }
                    else if (dataType == 1) {
                        for (int j = 0; j < tempString.length(); j++) {
                            if (tempString[j] < 48 || tempString[j] > 57) {
                                printf("Bad data assignment, at command %d\n", commandIndex);
                                exit(1);
                            }
                        }
                        tempVar.intValue = std::stoi(tempString);
                    }
                    else if (dataType == 2) {
                        for (int j = 0; j < tempString.length(); j++) {
                            if ((tempString[j] < 48 || tempString[j] > 57) && tempString[j] != '.') {
                                printf("Bad data assignment, at command %d\n", commandIndex);
                                exit(1);
                            }
                        }
                        tempVar.floatValue = std::stod(tempString);
                    }
                    else if (dataType == 3) {
                        if (std::count(tempString.begin(), tempString.end(), '"') != 2) {
                            printf("Bad \" count, at command %d\n", commandIndex);
                            exit(1);
                        }
                        if (tempString[0] != '"' || tempString.back() != '"') {
                            printf("Bad \" placement, at command %d\n", commandIndex);
                            exit(1);
                        }
                        tempString.erase(0, 1);
                        tempString.pop_back();
                        tempVar.stringValue = tempString;
                    }
                }
            }
        }
        bool nameExists = false;
        int foundVarIndex = 0;
        for (int i = 0; i < programVariables.size(); i++) {
            if (programVariables[i].varName == tempVar.varName) {
                nameExists = true;
                foundVarIndex = i;
                break;
            }
        }
        if (nameExists) {
            if (dataType == 0) {
                programVariables[foundVarIndex].boolValue = tempVar.boolValue;
            }
            else if (dataType == 1) {
                programVariables[foundVarIndex].intValue = tempVar.intValue;
            }
            else if (dataType == 2) {
                programVariables[foundVarIndex].floatValue = tempVar.floatValue;
            }
            else {
                programVariables[foundVarIndex].stringValue = tempVar.stringValue;
            }
        }
        else {
            programVariables.push_back(tempVar);
        }
    }
    else {
        std::cout << "Function command: " << command << std::endl;
    }
}
