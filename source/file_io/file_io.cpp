#include "file_io/file_io.h"

#include <fstream>
#include <sstream>


std::string faxdawn::file_io::read_string(const std::string& filepath)
{
	std::ifstream file(filepath);
	if (!file.is_open()) {
		return "";
	}
	std::stringstream data_stream;
	data_stream << file.rdbuf();
	file.close();
	return data_stream.str();
}

bool faxdawn::file_io::write_string(const std::string& filePath, const std::string& data)
{
	std::ofstream file(filePath);
	if (!file.is_open()) {
		return false;
	}
	file << data;
	file.close();
	return true;
}

int64_t faxdawn::file_io::read_bytes(const std::string& filePath, void* buffer, const size_t buffer_size)
{
	FILE* file = nullptr;
	if (fopen_s(&file, filePath.data(), "rb")) {
		return -1;
	}
	const int bytes_read = int(fread(buffer, 1, buffer_size, file));
	fclose(file);
	return bytes_read;
}

int64_t faxdawn::file_io::write_bytes(const std::string& filePath, const void* buffer, const size_t buffer_size)
{
	FILE* file = nullptr;
	if (fopen_s(&file, filePath.data(), "wb")) {
		return -1;
	}
	const int bytes_written = int(fwrite(buffer, 1, buffer_size, file));
	fclose(file);
	return bytes_written;
}
