#ifndef BOX_HPP
#define BOX_HPP

#include "MUtil.hpp"

namespace Box
{

    struct AABB{
        MUtil::Vec3 size;
        MUtil::Vec3 position;

        MUtil::Vec3 min() { return position - (size/2.0); }
        MUtil::Vec3 max() { return position + (size/2.0); }
    };

    bool collides(Box::AABB aabb, MUtil::Vec3 point);

} // namespace Box


#endif //BOX_HPP