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

bool faxdawn::file_io::write_string(const std::string& filepath, const std::string& data)
{
	std::ofstream file(filepath);
	if (!file.is_open()) {
		return false;
	}
	file << data;
	file.close();
	return true;
}

static FILE* open_file(const std::string& filepath, const std::string& mode)
{
	FILE* file = nullptr;
#ifdef _WIN32
	fopen_s(&file, filepath.data(), mode.c_str());
#else
	file = fopen(filepath.data(), mode.c_str());
#endif
	return file;
}

size_t faxdawn::file_io::read_bytes(const std::string& filepath, void* buffer, const size_t buffer_size)
{
	if (FILE* file = open_file(filepath, "rb")) {
        const size_t bytes_read = fread(buffer, 1, buffer_size, file);
        fclose(file);
        return bytes_read;
	}
    return -1;
}

size_t faxdawn::file_io::write_bytes(const std::string& filepath, const void* buffer, const size_t buffer_size)
{
	if (FILE* file = open_file(filepath, "wb")) {
        const size_t bytes_written = fwrite(buffer, 1, buffer_size, file);
        fclose(file);
        return bytes_written;
	}
	return -1;
}
