#include "Collider.hpp"

bool Collider::AABB::operator<(MLib::Vec3& v) const
{
    return !(*this >= v);
}

bool Collider::AABB::operator<=(MLib::Vec3& v) const
{
    return !(*this > v);
}

bool Collider::AABB::operator>(MLib::Vec3& v) const
{
    return v > min() && v < max();
}

bool Collider::AABB::operator>=(MLib::Vec3& v) const
{
    return v >= min() && v <= max();
}

bool Collider::AABB::operator&(const AABB &v) const
{
    MLib::Vec3 omin = v.min(), omax = v.max();
    MLib::Vec3 mmin = min(), mmax = max();
    return (mmin.x <= omax.x && mmax.x >= omin.x) &&
         (mmin.y <= omax.y && mmax.y >= omin.y) &&
         (mmin.z <= omax.z && mmax.z >= omin.z);
}

/*bool Collider::AABB::operator&(const Sphere &v) const
{
    MLib::Vec3 pmin = min(), pmax = max();
    MLib::Vec3 cx = {std::max(pmin.x, std::min(v.position.x, pmax.x)),
                    std::max(pmin.y, std::min(v.position.y, pmax.y)),
                    std::max(pmin.z, std::min(v.position.z, pmax.z))};
    return (cx - v.position).length2() <= v.radius * v.radius;
}*/

bool Collider::AABB::operator&(const Sphere &v) const
{
    const double min_x = position.x - size.x * 0.5;
    const double max_x = position.x + size.x * 0.5;
    const double min_y = position.y - size.y * 0.5;
    const double max_y = position.y + size.y * 0.5;
    const double min_z = position.z - size.z * 0.5;
    const double max_z = position.z + size.z * 0.5;

    const double cx = std::max(min_x, std::min(v.position.x, max_x));
    const double cy = std::max(min_y, std::min(v.position.y, max_y));
    const double cz = std::max(min_z, std::min(v.position.z, max_z));

    const double dist2 = (cx - v.position.x) * (cx - v.position.x) +
                         (cy - v.position.y) * (cy - v.position.y) +
                         (cz - v.position.z) * (cz - v.position.z);

    return dist2 <= v.radius * v.radius;
}

bool Collider::Sphere::operator<(MLib::Vec3 &v) const
{
    return (this->position - v).length2() > this->radius * this->radius;
}

bool Collider::Sphere::operator<=(MLib::Vec3 &v) const
{
    return (this->position - v).length2() >= this->radius * this->radius;
}

bool Collider::Sphere::operator>(MLib::Vec3 &v) const
{
    return (this->position - v).length2() < this->radius * this->radius;
}

bool Collider::Sphere::operator>=(MLib::Vec3 &v) const
{
    return (this->position - v).length2() <= this->radius * this->radius;
}

bool Collider::Sphere::operator&(const AABB &v) const
{
    return v & *this;
}

bool Collider::Sphere::operator&(const Sphere &v) const
{
    double r2 = radius + v.radius;
    r2 *= r2;
    return (position - v.position).length2() <= r2;
}
