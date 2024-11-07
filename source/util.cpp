#include "util.h"


dawn::String dawn::read_file(const StringRef& path)
{
	IFileStream file{ path.data() };
	if (!file)
		return {};
	return (StringStream{} << file.rdbuf()).str();
}
