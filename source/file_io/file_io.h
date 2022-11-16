#pragma once

#include <string>


namespace faxdawn::file_io {
	std::string read_string(const std::string& filepath);
	bool write_string(const std::string& filepath, const std::string& data);

	int64_t read_bytes(const std::string& filepath, void* buffer, size_t buffer_size);
	int64_t write_bytes(const std::string& filepath, const void* buffer, size_t buffer_size);
}
