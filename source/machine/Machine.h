#pragma once

#include "logic/Function.h"


namespace dawn {
	class Machine : public Map<String, Function> {
	public:
		void Load(const String& name, const Function& function);
		void Unload(const String& name);

		bool Execute(const String& name);
		bool ExecuteMain();
	};
}
