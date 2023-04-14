#include "machine/Machine.h"


void dawn::Machine::Load(const String& name, const Function& function) {
	(*this)[name] = function;
}

void dawn::Machine::Unload(const String& name) {
	this->erase(name);
}

bool dawn::Machine::Execute(const String& name) {
	if (!this->contains(name)) {
		return false;
	}

	this->at(name).Execute();
	return true;
}

bool dawn::Machine::ExecuteMain() {
	return Execute(L"main");
}
