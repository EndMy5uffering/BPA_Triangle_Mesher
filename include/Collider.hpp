#ifndef BOX_HPP
#define BOX_HPP

#include <glm.hpp>
#include <algorithm>
#include "gtx/norm.hpp"

namespace Collider
{

    struct AABB;
    struct Sphere;

    struct AABB{
        glm::vec3 position;
        glm::vec3 size;

        glm::vec3 min() const { return position - (size/2.0f); }
        glm::vec3 max() const { return position + (size/2.0f); }

        bool operator<(glm::vec3& v) const;
        bool operator<=(glm::vec3& v) const;
        bool operator>(glm::vec3& v) const;
        bool operator>=(glm::vec3& v) const;
        
        bool operator&(const AABB& v) const;

        bool operator&(const Sphere& v) const;
    };

    struct Sphere{
        glm::vec3 position;
        double radius;

        bool operator<(glm::vec3& v) const;
        bool operator<=(glm::vec3& v) const;
        bool operator>(glm::vec3& v) const;
        bool operator>=(glm::vec3& v) const;

        bool operator&(const AABB& v) const;

        bool operator&(const Sphere& v) const;
    };

} // namespace Box


#endif //BOX_HPP