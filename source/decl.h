#pragma once

#include "en.h"


namespace dawn
{
struct Location;
struct Value;
struct Node;
struct Frame;
struct PopHandler;
struct Engine;

template<typename T>
struct RegisterRef;

using NodeRef = RegisterRef<Node>;
using MemberGenerator = Func<Value( Location const&, Value const& )>;
}
