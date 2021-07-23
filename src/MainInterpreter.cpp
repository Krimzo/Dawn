#include <iostream>
#include <string>
#include <fstream>

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
                printf("File opened!\n");

                openedCodeFile.close();
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