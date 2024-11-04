#include "Box.hpp"

bool Box::contains(Box::AABB aabb, MLib::Vec3 point)
{
    return point > aabb.min() && point < aabb.max();
}
