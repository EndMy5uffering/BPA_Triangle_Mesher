//PROJECT WAS CREATED WITH PYMAKE
//PYMAKE: https://github.com/EndMy5uffering/PyCmakeToolKit
//THIS FILE CAN BE CHANGED TO FIT YOUR PROJECT. THIS IS ONLY A TEMPLATE FOR A QUICK SETUP.
#ifndef HELLOWORLD_HPP
#define HELLOWORLD_HPP

#include <string>
#include <iostream>

class HelloWorld
{
private:
    std::string m_text;

public:
    HelloWorld(std::string text);
    ~HelloWorld();

    void Print();
};

#endif // HELLOWORLD_HPP
