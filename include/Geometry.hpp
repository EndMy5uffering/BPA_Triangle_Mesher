#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include "MLib.hpp"

namespace Geometry
{
    struct Vertex{
        MLib::Vec3 position;
        MLib::Vec3 normal;
        MLib::Vec2 texture;

        Vertex(MLib::Vec3 _position, MLib::Vec3 _normal, MLib::Vec2 _texture)
        : position{_position}, normal{_normal}, texture{_texture}
        {}

        bool operator==(Vertex& rhs)
        {
            return position == rhs.position && normal == rhs.normal && texture == rhs.texture;
        }
    };

    struct Triangle{

        int v1_idx;
        int v2_idx;
        int v3_idx;

    };
}

#endif //GEOMETRY_HPP