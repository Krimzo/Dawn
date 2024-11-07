#pragma once

#include "t.h"


namespace dawn {
	Char to_escaping(Char c);
	String from_escaping(Char c);
	String read_file(const StringRef& path);
}
