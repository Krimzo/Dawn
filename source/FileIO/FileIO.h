#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


namespace Faxdawn {
	class FileIO {
	public:
		FileIO();
		FileIO(const FileIO&) = delete;
		void operator=(const FileIO&) = delete;
		~FileIO();

		std::string read(const std::string& filePath) const;
		bool write(const std::string& filePath, const std::string& data) const;

		int read(const std::string& filePath, void* buffer, int bufferSize) const;
		int write(const std::string& filePath, const void* buffer, int bufferSize) const;
	};
}
