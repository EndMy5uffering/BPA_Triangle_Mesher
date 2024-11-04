#ifndef BOX_HPP
#define BOX_HPP

#include "MLib.hpp"

namespace Box
{

    struct AABB{
        MLib::Vec3 position;
        MLib::Vec3 size;

        MLib::Vec3 min() { return position - (size/2.0); }
        MLib::Vec3 max() { return position + (size/2.0); }
    };

    bool contains(Box::AABB aabb, MLib::Vec3 point);

} // namespace Box


#endif //BOX_HPP