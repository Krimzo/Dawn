#include "Faxdawn.h"
#include "Utility/Utility.h"
#include "Interpreter/LineHandler.h"


int main(int argc, char** argv) {

#ifdef FaxdawnDebug
	argc = 2;
	argv[1] = (char*)"test/test.xdn";
#endif

	if (argc < 2) {
		std::cout << "Error.Source file input is empty" << std::endl;
	}
	else if (argc > 2) {
		std::cout << "Error. Too many arguments given" << std::endl;
	}
	else {
		const String fileName(argv[1]);
		if (fileName.length() > Faxdawn::fileExtension.length() && fileName.find(Faxdawn::fileExtension) == (fileName.length() - Faxdawn::fileExtension.length())) {
			std::ifstream fileStream(fileName);
			if (fileStream.is_open()) {
				std::stringstream dataStream;
				dataStream << fileStream.rdbuf();
				fileStream.close();
				Utility::ClearSource(dataStream);
				Faxdawn::ProcessLines(dataStream);
			}
			else {
				std::cout << "Error. File \"" << fileName << "\" does not exist" << std::endl;
			}
		}
		else {
			std::cout << "Error. File \"" << fileName << "\" does not contain a \"" << Faxdawn::fileExtension << "\" extension" << std::endl;
		}
	}

#ifdef FaxdawnDebug
	std::cin.get();
#endif

}
