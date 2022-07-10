#pragma once

#include <string>


namespace Faxdawn {
	class IOHandler {
	public:
		IOHandler();
		IOHandler(const IOHandler&) = delete;
		void operator=(const IOHandler&) = delete;
		~IOHandler();

		std::string read(const std::string& filePath) const;
		bool write(const std::string& filePath, const std::string& data) const;

		int read(const std::string& filePath, void* buffer, int bufferSize) const;
		int write(const std::string& filePath, const void* buffer, int bufferSize) const;
	};
}
