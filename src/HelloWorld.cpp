//PROJECT WAS CREATED WITH PYMAKE
//PYMAKE: https://github.com/EndMy5uffering/PyCmakeToolKit
//THIS FILE CAN BE CHANGED TO FIT YOUR PROJECT. THIS IS ONLY A TEMPLATE FOR A QUICK SETUP.
#include "HelloWorld.hpp"

HelloWorld::HelloWorld(std::string text) 
:
m_text{text}
{
}

HelloWorld::~HelloWorld() 
{
}

void HelloWorld::Print() 
{
    std::cout << "From Hello World: " << m_text << "\n";
}

