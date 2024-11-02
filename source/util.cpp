#include "util.h"


dawn::String dawn::read_file(const StringView& path)
{
	IFileStream file{ path.data() };
	if (!file)
		return {};
	return (StringStream{} << file.rdbuf()).str();
}
