#include "file_helper.h"


std::string dawn::read_file(const std::string& path)
{
	std::stringstream stream = {};
	stream << std::ifstream(path).rdbuf();
	return stream.str();
}
