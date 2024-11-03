#pragma once

#include "lexer.h"


namespace dawn {
	struct Node
	{
		virtual ~Node() = default;
	};
}

namespace dawn {
	using Tree = Ref<Node>;
}

namespace dawn {
	struct Parser
	{
		Opt<String> parse(const Array<Token>& tokens, Tree& tree) const;
	};
}
