#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "MyStuff/MyGlobal.h"
#include "MyStuff/MyStruct.h"
#include "MyStuff/MyFunc.h"


std::vector<std::string> GetCommands(std::string fileAsString) {
    std::vector<std::string> commands;

    std::string tempString;
    int bracketCounter = 0;
    int newLineCount = 0;
    bool commentFound = false;
    for (int i = 0; i < fileAsString.length(); i++) {
        if (!commentFound) {
            switch (fileAsString[i]) {
            case 9:
                break;
            case '\n':
                if (bracketCounter == 0) {
                    if (tempString != "") {
                        while (tempString.find("  ") != -1) {
                            tempString.replace(tempString.find("  "), 2, " ");
                        }
                        if (tempString != " ") {
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
                tempString.push_back(fileAsString[i]);
                bracketCounter++;
                break;
            case '}':
                tempString.push_back(fileAsString[i]);
                bracketCounter--;
                if (bracketCounter == 0) {
                    while (tempString.find("~~") != -1) {
                        tempString.replace(tempString.find("~~"), 2, "~");
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
                    commands.push_back(tempString);
                    tempString.clear();
                }
                else if (bracketCounter < 0) {
                    printf("Excess '}' found at line %d\n", newLineCount + 1);
                    exit(1);
                }
                break;
            case ';':
                printf("FaxDown doesn't use ';' found at line %d\n", newLineCount + 1);
                exit(1);
                break;
            case '$':
                if (bracketCounter == 0) {
                    if (tempString != "") {
                        while (tempString.find("  ") != -1) {
                            tempString.replace(tempString.find("  "), 2, " ");
                        }
                        commands.push_back(tempString);
                    }
                    tempString.clear();
                }
                commentFound = true;
                break;
            default:
                tempString.push_back(fileAsString[i]);
                break;
            }
        }
        else if (fileAsString[i] == '\n') {
            commentFound = false;
        }
    }

    if (bracketCounter != 0) {
        printf("Excess '{' found\n");
        exit(1);
    }

    return commands;
}

