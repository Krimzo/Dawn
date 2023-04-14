#pragma once

#include "dawn/CppTypes.h"


namespace dawn {
	String ReadFileString(const String& filepath);
	bool WriteFileString(const String& filepath, const String& data);

	Int ReadFileBytes(const String& filepath, void* buffer, Int buffer_size);
	Int WriteFileBytes(const String& filepath, const void* buffer, Int buffer_size);

	FILE* OpenFileC(const String& filepath, const String& mode);
	Bool CloseFileC(FILE*& file);
}
