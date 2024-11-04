#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <Geometry.hpp>

namespace OBJ
{

    struct OBJData
    {
        std::vector<Vertex> vertexList;
        std::vector<unsigned int> indexBuffer;
    };

    void load(const char* path);
    void save();

}

#endif //OBJLOADER_HPP