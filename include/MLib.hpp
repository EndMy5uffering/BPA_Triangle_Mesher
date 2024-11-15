#ifndef MLIB_HPP
#define MLIB_HPP

#include <math.h>

namespace MLib
{

    template<typename T>
    inline const T& min(const T& a, const T& b)
    {
        return a < b ? a : b;
    }

    template<typename T>
    inline const T& max(const T& a, const T& b)
    {
        return a > b ? a : b;
    }

    struct Vec3{
        double x;
        double y;
        double z;

        double length() const
        {
            return sqrt((x*x) + (y*y) + (z*z));
        }

        double length2() const
        {
            return (x*x) + (y*y) + (z*z);
        }

        Vec3 norm() const
        {
            double l = length2();
            return Vec3{x/l, y/l, z/l};
        }

        double dot(const Vec3& rhs) const
        {
            return x * rhs.x + y * rhs.y + z * rhs.z;
        }

        Vec3 cross(const Vec3& rhs) const
        {
            return {(y * rhs.z) - (z * rhs.y), (z * rhs.x) - (x * rhs.z), (x * rhs.y) - (y * rhs.x)};
        }

        Vec3 operator+(const Vec3& rhs) const
        {
            return Vec3{x + rhs.x, y + rhs.y, z + rhs.z};
        }

        Vec3 operator-(const Vec3& rhs) const
        {
            return Vec3{x - rhs.x, y - rhs.y, z - rhs.z};
        }

        template<typename T>
        Vec3 operator/(T rhs) const
        {
            return Vec3{x / rhs, y / rhs, z / rhs};
        }

        template<typename T>
        Vec3& operator/=(T rhs) const
        {
            x /= rhs;
            y /= rhs; 
            z /= rhs;
            return *this;
        }

        template<typename T>
        Vec3 operator*(T rhs) const
        {
            return Vec3{x * rhs, y * rhs, z * rhs};
        }

        template<typename T>
        Vec3& operator*=(T rhs) const
        {
            x *= rhs;
            y *= rhs; 
            z *= rhs;
            return *this;
        }

        bool operator<(Vec3& rhs) const
        {
            return x < rhs.x && y < rhs.y && z < rhs.z;
        }

        bool operator>(Vec3& rhs) const
        {
            return x > rhs.x && y > rhs.y && z > rhs.z;
        }

        bool operator>=(Vec3& rhs) const
        {
            return x >= rhs.x && y >= rhs.y && z >= rhs.z;
        }

        bool operator<=(Vec3& rhs) const
        {
            return x <= rhs.x && y <= rhs.y && z <= rhs.z;
        }

        bool operator==(Vec3& rhs) const
        {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }
    };

    static double dot(const Vec3& lhs, const Vec3& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    static Vec3 cross(const Vec3& lhs, const Vec3& rhs)
    {
        return {(lhs.y * rhs.z) - (lhs.z * rhs.y), (lhs.z * rhs.x) - (lhs.x * rhs.z), (lhs.x * rhs.y) - (lhs.y * rhs.x)};
    }

    struct Vec2{
        double x;
        double y;

        Vec2 operator+(const Vec2& rhs)
        {
            return Vec2{x + rhs.x, y + rhs.y};
        }

        Vec2 operator-(const Vec2& rhs)
        {
            return Vec2{x - rhs.x, y - rhs.y};
        }

        template<typename T>
        Vec2 operator/(T rhs)
        {
            return Vec2{x / rhs, y / rhs};
        }

        template<typename T>
        Vec2& operator/=(T rhs)
        {
            x /= rhs;
            y /= rhs; 
            return *this;
        }

        template<typename T>
        Vec2 operator*(T rhs)
        {
            return Vec2{x * rhs, y * rhs};
        }

        template<typename T>
        Vec2& operator*=(T rhs)
        {
            x *= rhs;
            y *= rhs; 
            return *this;
        }

        bool operator<(const Vec2& rhs)
        {
            return x < rhs.x && y < rhs.y;
        }

        bool operator>(const Vec2& rhs)
        {
            return x > rhs.x && y > rhs.y;
        }

        bool operator>=(const Vec2& rhs)
        {
            return x >= rhs.x && y >= rhs.y;
        }

        bool operator<=(const Vec2& rhs)
        {
            return x <= rhs.x && y <= rhs.y;
        }

        bool operator==(const Vec2& rhs)
        {
            return x == rhs.x && y == rhs.y;
        }
    };

    inline bool LineLineIntersection(Vec3 p0, Vec3 d0, Vec3 p1, Vec3 d1, Vec3* poi) {
        Vec3 p0p1 = p1 - p0;
        double d0d0 = d0.dot(d0);
        double d1d1 = d1.dot(d1);
        double d0d1 = d0.dot(d1);
        double p0p1d0 = p0p1.dot(d0);
        double p0p1d1 = p0p1.dot(d1);

        double denom = d0d0 * d1d1 - d0d1 * d0d1;

        if (fabs(denom) < 1e-6) return false; 

        double t = (p0p1d1 * d0d1 - p0p1d0 * d1d1) / denom;
        double s = (p0p1d1 + t * d0d1) / d1d1;

        Vec3 closestPointLine1 = p0 + d0 * t;
        Vec3 closestPointLine2 = p1 + d1 * s;

        Vec3 i = (closestPointLine1 + closestPointLine2) * 0.5;
        poi->x = i.x;
        poi->y = i.y;
        poi->z = i.z;
        return true;
    }

} // namespace MLib


#endif //MLIB_HPP