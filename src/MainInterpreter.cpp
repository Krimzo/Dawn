#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include "MyStuff/MyGlobal.h"
#include "MyStuff/MyStruct.h"
#include "MyStuff/MyFunc.h"


std::vector<FaxDownVar> programVariables;

int main(int argc, char* argv[]) {
    // Check argument count
    if (argc == 2) {
        // Convert C char* to cpp std::string
        std::string fileName(*(argv + 1));
        // Check if 2. argument has .fxdn extension
        if (fileName.length() > 5 && fileName.find(".fxdn") == (fileName.length() - 5)) {
            // Code file openning
            std::ifstream openedCodeFile(fileName);
            // Check if file exists
            if (openedCodeFile.is_open()) {
                // Read commands to string vector and close the file stream
                std::stringstream stringBuffer;
                stringBuffer << openedCodeFile.rdbuf();
                std::vector<std::string> programCommands = GetCommands(stringBuffer.str());
                openedCodeFile.close();

                // Handle commands
                for(int i=0; i<programCommands.size(); i++){
                    HandleCommand(programCommands[i]);
                }
            }
            else {
                printf("File doesn't exist.\n");
                return 1;
            }
        }
        else {
            printf("Missing file extension .fxdn or the file name doesn't exist.\n");
        }
    }
    else if (argc > 2) {
        printf("Too many arguments given.\n");
        return 1;
    }
    else {
        printf("Missing file argument.\n");
        return 1;
    }
    return 0;
}