#pragma once

#include "parser.h"


namespace dawn {
	struct Engine
	{
		Opt<String> eval(const Tree& tree, Any& result) const;
	};
}
