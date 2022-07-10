#include "IOHandler/IOHandler.h"

#include <fstream>
#include <sstream>


Faxdawn::IOHandler::IOHandler() {

}
Faxdawn::IOHandler::~IOHandler() {

}

std::string Faxdawn::IOHandler::read(const std::string& filePath) const {
	std::ifstream file(filePath);
	if (!file.is_open()) {
		return "";
	}
	std::stringstream dataStream;
	dataStream << file.rdbuf();
	file.close();
	return dataStream.str();
}

bool Faxdawn::IOHandler::write(const std::string& filePath, const std::string& data) const {
	std::ofstream file(filePath);
	if (!file.is_open()) {
		return false;
	}
	file << data;
	file.close();
	return true;
}

int Faxdawn::IOHandler::read(const std::string& filePath, void* buffer, int bufferSize) const {
	FILE* file = nullptr;
	if (fopen_s(&file, filePath.data(), "rb")) {
		return 0;
	}
	int bytesRead = int(fread(buffer, 1, bufferSize, file));
	fclose(file);
	return bytesRead;
}
int Faxdawn::IOHandler::write(const std::string& filePath, const void* buffer, int bufferSize) const {
	FILE* file = nullptr;
	if (fopen_s(&file, filePath.data(), "wb")) {
		return 0;
	}
	int bytesWritten = int(fwrite(buffer, 1, bufferSize, file));
	fclose(file);
	return bytesWritten;
}
