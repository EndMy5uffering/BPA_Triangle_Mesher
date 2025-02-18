#include "Collider.hpp"

bool Collider::AABB::operator<(glm::vec3& v) const
{
    return !(*this >= v);
}

bool Collider::AABB::operator<=(glm::vec3& v) const
{
    return !(*this > v);
}

bool Collider::AABB::operator>(glm::vec3& v) const
{
    glm::vec3 vmin = min();
    glm::vec3 vmax = max();
    return v.x > vmin.x && v.y > vmin.y && v.z > vmin.z && v.x < vmax.x && v.y < vmax.y && v.y < vmax.y;
}

bool Collider::AABB::operator>=(glm::vec3& v) const
{
    glm::vec3 vmin = min();
    glm::vec3 vmax = max();
    return v.x >= vmin.x && v.y >= vmin.y && v.z >= vmin.z && v.x <= vmax.x && v.y <= vmax.y && v.y <= vmax.y;
}

bool Collider::AABB::operator&(const AABB &v) const
{
    glm::vec3 omin = v.min(), omax = v.max();
    glm::vec3 mmin = min(), mmax = max();
    return (mmin.x <= omax.x && mmax.x >= omin.x) &&
         (mmin.y <= omax.y && mmax.y >= omin.y) &&
         (mmin.z <= omax.z && mmax.z >= omin.z);
}

/*bool Collider::AABB::operator&(const Sphere &v) const
{
    glm::vec3 pmin = min(), pmax = max();
    glm::vec3 cx = {std::max(pmin.x, std::min(v.position.x, pmax.x)),
                    std::max(pmin.y, std::min(v.position.y, pmax.y)),
                    std::max(pmin.z, std::min(v.position.z, pmax.z))};
    return (cx - v.position).length2() <= v.radius * v.radius;
}*/

bool Collider::AABB::operator&(const Sphere &v) const
{
    const float min_x = position.x - size.x * 0.5f;
    const float max_x = position.x + size.x * 0.5f;
    const float min_y = position.y - size.y * 0.5f;
    const float max_y = position.y + size.y * 0.5f;
    const float min_z = position.z - size.z * 0.5f;
    const float max_z = position.z + size.z * 0.5f;

    const float cx = std::max(min_x, std::min(v.position.x, max_x));
    const float cy = std::max(min_y, std::min(v.position.y, max_y));
    const float cz = std::max(min_z, std::min(v.position.z, max_z));

    const float dist2 = (cx - v.position.x) * (cx - v.position.x) +
                         (cy - v.position.y) * (cy - v.position.y) +
                         (cz - v.position.z) * (cz - v.position.z);

    return dist2 <= v.radius * v.radius;
}

bool Collider::Sphere::operator<(glm::vec3 &v) const
{
    return glm::length2((this->position - v)) > this->radius * this->radius;
}

bool Collider::Sphere::operator<=(glm::vec3 &v) const
{
    return glm::length2((this->position - v)) >= this->radius * this->radius;
}

bool Collider::Sphere::operator>(glm::vec3 &v) const
{
    return glm::length2((this->position - v)) < this->radius * this->radius;
}

bool Collider::Sphere::operator>=(glm::vec3 &v) const
{
    return glm::length2((this->position - v)) <= this->radius * this->radius;
}

bool Collider::Sphere::operator&(const AABB &v) const
{
    return v & *this;
}

bool Collider::Sphere::operator&(const Sphere &v) const
{
    double r2 = radius + v.radius;
    r2 *= r2;
    return (position - v.position).length() <= r2;
}
