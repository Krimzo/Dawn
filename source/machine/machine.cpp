#include "machine/machine.h"

#include "faxdawn/utility.h"
#include "file_io/file_io.h"


void faxdawn::machine::load(const std::string& name, const function& function)
{
	functions_[name] = function;
}

void faxdawn::machine::remove(const std::string& name)
{
	functions_.erase(name);
}

bool faxdawn::machine::execute(const std::string& name) const
{
	if (functions_.contains(name)) {
		functions_.at(name).execute();
		return true;
	}
	return false;
}

bool faxdawn::machine::execute_main() const 
{
	return execute("main");
}
