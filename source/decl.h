#pragma once

#include "en.h"


namespace dawn
{
struct Value;
struct Node;
struct Frame;
struct PopHandler;
struct Engine;

template<typename T>
struct RegisterRef;

using NodeRef = RegisterRef<Node>;
using CFuncBody = Func<Value( Value*, Int )>;
using MemberGenerator = Func<Value( Value const& )>;
}
