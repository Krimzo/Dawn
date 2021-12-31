#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>

#include "MiscFuncs.h"
#include "LineHandler.h";

const std::string fileExtension = ".xdn";

int main(int argc, char** argv) {
    // Checking arguments
    if (argc < 2) {
        printf("Error. Source file input is empty.\n");
        return 1;
    }
    else if (argc > 2) {
        printf("Error. Too many arguments given.\n");
        return 1;
    }
    else {
        // Getting the file name
        std::string fileName(argv[1]);

        // Checking if the file has .xdn extension
        if (fileName.length() > fileExtension.length() && fileName.find(fileExtension) == (fileName.length() - fileExtension.length())) {
            // Opening source file
            std::ifstream fileStream(fileName);

            // Checking the file status
            if (fileStream.is_open()) {
                // Reading the file to a string
                std::stringstream sourceStream;
                sourceStream << fileStream.rdbuf();
                std::string source = sourceStream.str();

                // Closing the file stream
                fileStream.close();

                // Removing whitespace
                while (StringReplace(source, "\t", " "));
                while (StringReplace(source, "  ", " "));
                while (StringReplace(source, "\n ", "\n"));
                if (source.front() == ' ') source.erase(source.begin());

                // Getting a new source stream
                sourceStream = std::stringstream(source);

                // Interpreting lines
                std::string line;
                while (std::getline(sourceStream, line)) HandleLine(line);
            }
            else {
                printf("Error. File \"%s\" does not exist.\n", fileName.c_str());
                return 1;
            }
        }
        else {
            printf("Error. File \"%s\" does not contain a \".xdn\" extension.\n", fileName.c_str());
        }
    }
    return 0;
}