#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "MLib.hpp"

namespace Geometry
{
    struct Vertex{
        MLib::Vec3 position;
        MLib::Vec3 normal;
        MLib::Vec2 texture;

        bool operator==(Vertex& rhs)
        {
            return position == rhs.position && normal == rhs.normal && texture == rhs.texture;
        }
    };

    struct Edge{
        Vertex& v1;
        Vertex& v2;

        bool operator==(Edge& rhs)
        {
            return (v1 == rhs.v1 || v1 == rhs.v2) && (v2 == rhs.v1 || v2 == rhs.v2);
        }
    };

    struct Triangle{

        Vertex& v1;
        Vertex& v2;
        Vertex& v3;

        Edge& e1;
        Edge& e2;
        Edge& e3;

    };
}

#endif //GEOMETRY_HPP