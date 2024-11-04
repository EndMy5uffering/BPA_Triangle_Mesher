//PROJECT WAS CREATED WITH PYMAKE
//PYMAKE: https://github.com/EndMy5uffering/PyCmakeToolKit
//THIS FILE CAN BE CHANGED TO FIT YOUR PROJECT. THIS IS ONLY A TEMPLATE FOR A QUICK SETUP.

#include <Octree.hpp>

#include <iostream>

#include <ObjLoader.hpp>

void TreeTest()
{
    Octree tree{{{0,0,0}, {10,10,10}}};

    Vertex v{4, 0, 1, 1,0,0};
    Vertex v2{4.8, 3, 4, 1,0,0};
    Vertex v3{4.9, 3.1, 4, 1,0,0};

    tree.insert(&v);
    tree.insert(&v2);
    tree.insert(&v3);

    tree.print();

    std::cout << "done\n";
}

void OBJLoaderTest()
{
    const char* path = "D:\\teapot.obj";
    OBJ::load(path);
}

int main(void) 
{
    
    OBJLoaderTest();
}

