#ifndef BOX_HPP
#define BOX_HPP

#include "MLib.hpp"
#include <algorithm>

namespace Collider
{

    struct AABB;
    struct Sphere;

    struct AABB{
        MLib::Vec3 position;
        MLib::Vec3 size;

        MLib::Vec3 min() const { return position - (size/2.0f); }
        MLib::Vec3 max() const { return position + (size/2.0f); }

        bool operator<(MLib::Vec3& v) const;
        bool operator<=(MLib::Vec3& v) const;
        bool operator>(MLib::Vec3& v) const;
        bool operator>=(MLib::Vec3& v) const;
        
        bool operator&(const AABB& v) const;

        bool operator&(const Sphere& v) const;
    };

    struct Sphere{
        MLib::Vec3 position;
        double radius;

        bool operator<(MLib::Vec3& v) const;
        bool operator<=(MLib::Vec3& v) const;
        bool operator>(MLib::Vec3& v) const;
        bool operator>=(MLib::Vec3& v) const;

        bool operator&(const AABB& v) const;

        bool operator&(const Sphere& v) const;
    };

} // namespace Box


#endif //BOX_HPP