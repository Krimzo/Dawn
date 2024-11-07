#include "util.h"


dawn::Char dawn::to_escaping(const Char c)
{
	switch (c)
	{
	case L'b': return L'\b';
	case L'f': return L'\f';
	case L'n': return L'\n';
	case L'r': return L'\r';
	case L't': return L'\t';
	}
	return c;
}

dawn::String dawn::from_escaping(const Char c)
{
	switch (c)
	{
	case L'\b': return L"\\b";
	case L'\f': return L"\\f";
	case L'\n': return L"\\n";
	case L'\r': return L"\\r";
	case L'\t': return L"\\t";
	}
	return String(1, c);
}

dawn::String dawn::read_file(const StringRef& path)
{
	IFileStream file{ path.data() };
	if (!file)
		return {};
	return (StringStream{} << file.rdbuf()).str();
}
