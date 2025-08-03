#pragma once

#include "t.h"


namespace dawn
{
struct Engine;
struct Frame;
struct PopHandler;
struct Node;
struct Value;

template<typename T>
struct RegisterRef;

using MemberGenerator = Func<Value( Value const& )>;
using CFuncBody = Func<Value( Value*, Int )>;
using NodeRef = RegisterRef<Node>;
}
