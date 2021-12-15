#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>


int main(int argc, char** argv) {
    // Checking argument count
    if (argc > 2) {
        printf("Too many arguments given.\n");
        return 1;
    }
    else if (argc < 2) {
        printf("Missing file argument.\n");
        return 1;
    }
    else {
        // Getting the file name
        std::string fileName(argv[1]);

        // Check if the file has .fxdn extension
        if (fileName.length() > 5 && fileName.find(".fxdn") == (fileName.length() - 5)) {
            // Opening the source file
            std::ifstream sourceFile(fileName);

            // Opening the file
            if (sourceFile.is_open()) {
                // Closing the file stream
                sourceFile.close();
            }
            else {
                printf("File doesn't exist.\n");
                return 1;
            }
        }
        else {
            printf("Missing file extension .fxdn\n");
        }
    }
    return 0;
}