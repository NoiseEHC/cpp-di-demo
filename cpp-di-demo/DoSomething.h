#pragma once

#include "IDoSomething.h"
#include <iostream>

class DoSomethinger final : public IDoSomething
{
    int a_;
    std::string b_;

public:
    explicit DoSomethinger(int a, std::string b) : a_{a}, b_{std::move(b)}
    {
    }

    void DoIt(int c, std::string const& d) const override;
};

inline void DoSomethinger::DoIt(int c, std::string const& d) const
{
    // and here do something with a_, b_, c and d
    std::cout << a_ << ", " << b_ << ", " << c << ", " << d << std::endl;
}
