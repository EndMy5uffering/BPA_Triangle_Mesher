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

    struct FaceData{
        std::vector<int> vertexIDXList;
        std::vector<int> vertexTextureIDXList;
        std::vector<int> vertexNormalIDXList;
    };

    void load(const char* path);
    void save();

}

#endif //OBJLOADER_HPP