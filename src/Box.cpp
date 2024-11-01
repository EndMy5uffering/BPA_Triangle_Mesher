#include "Box.hpp"

bool Box::collides(Box::AABB aabb, MUtil::Vec3 point)
{
    return point > aabb.min() && point < aabb.max();
}
