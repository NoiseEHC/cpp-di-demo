#pragma once

#include <string>

struct IDoSomething
{
	virtual ~IDoSomething() = default;
	virtual void DoIt(int c, std::string const& d) const = 0;
};
