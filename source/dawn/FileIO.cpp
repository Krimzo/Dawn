#include "dawn/FileIO.h"


dawn::String dawn::ReadFileString(const String& filepath) {
	std::wifstream file(filepath);
	if (!file.is_open()) {
		return {};
	}

	StringStream stream = {};
	stream << file.rdbuf();
	file.close();
	return stream.str();
}

dawn::Bool dawn::WriteFileString(const String& filepath, const String& data) {
	std::wofstream file(filepath);
	if (!file.is_open()) {
		return false;
	}

	file << data;
	file.close();
	return true;
}

dawn::Int dawn::ReadFileBytes(const String& filepath, void* buffer, const Int buffer_size) {
	FILE* file = OpenFileC(filepath, L"rb");
	if (!file) return -1;

	const Int bytes_read = (Int) fread(buffer, 1, buffer_size, file);
	CloseFileC(file);
	return bytes_read;
}

dawn::Int dawn::WriteFileBytes(const String& filepath, const void* buffer, const Int buffer_size) {
	FILE* file = OpenFileC(filepath, L"wb");
	if (!file) return -1;

	const size_t bytes_written = (Int) fwrite(buffer, 1, buffer_size, file);
	CloseFileC(file);
	return bytes_written;
}

FILE* dawn::OpenFileC(const String& filepath, const String& mode) {
	FILE* file = nullptr;
	_wfopen_s(&file, filepath.data(), mode.c_str());
	return file;
}

dawn::Bool dawn::CloseFileC(FILE*& file) {
	if (!file) {
		return false;
	}

	fclose(file);
	file = nullptr;
	return true;
}
