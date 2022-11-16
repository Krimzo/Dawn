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

size_t faxdawn::file_io::read_bytes(const std::string& filePath, void* buffer, const size_t buffer_size)
{
	if (FILE* file = fopen(filePath.data(), "rb")) {
        const size_t bytes_read = fread(buffer, 1, buffer_size, file);
        fclose(file);
        return bytes_read;
	}
    return -1;
}

size_t faxdawn::file_io::write_bytes(const std::string& filePath, const void* buffer, const size_t buffer_size)
{
	if (FILE* file = fopen(filePath.data(), "wb")) {
        const size_t bytes_written = fwrite(buffer, 1, buffer_size, file);
        fclose(file);
        return bytes_written;
	}
	return -1;
}
