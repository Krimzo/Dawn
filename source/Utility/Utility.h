#pragma once

#include "KrimzLib.h"


namespace Utility {
	size_t ReplaceInString(String& str, const String& from, const String& to);
	void ClearSource(std::stringstream& src);
}
