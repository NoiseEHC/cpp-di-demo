#pragma once

#include "IDoSomething.h"
#include <memory>

class CallSomethinger
{
    std::unique_ptr<IDoSomething const> doSomethinger_;

public:
	explicit CallSomethinger(std::unique_ptr<IDoSomething const>&& doSomethinger);
    void CallIt(int times) const;
};

inline CallSomethinger::CallSomethinger(std::unique_ptr<IDoSomething const>&& doSomethinger):
    doSomethinger_{std::move(doSomethinger)}
{
}

inline void CallSomethinger::CallIt(int times) const
{
    for (int i = 0; i < times; ++i)
        doSomethinger_->DoIt(i, "from caller");
}
