#ifndef MLIB_HPP
#define MLIB_HPP

#include <math.h>
#include <cmath>

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

        double dist(const Vec3& other) const
        {
            double _x = x - other.x;
            double _y = y - other.y;
            double _z = z - other.z;
            return sqrt((_x*_x) + (_y*_y) + (_z*_z));
        }

        Vec3 norm() const
        {
            double l = length();
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

        Vec3 operator-() const
        {
            return Vec3{-x, -y, -z};
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

        /**
         * @brief Dot product
         * 
         * @param rhs Other vector
         * @return double 
         */
        double operator*(const Vec3& rhs) const
        {
            return x * rhs.x + y * rhs.y + z * rhs.z;
        }

        template<typename T>
        Vec3& operator*=(T rhs)
        {
            x *= rhs;
            y *= rhs; 
            z *= rhs;
            return *this;
        }

        bool operator<(const Vec3& rhs) const
        {
            return x < rhs.x && y < rhs.y && z < rhs.z;
        }

        bool operator>(const Vec3& rhs) const
        {
            return x > rhs.x && y > rhs.y && z > rhs.z;
        }

        bool operator>=(const Vec3& rhs) const
        {
            return x >= rhs.x && y >= rhs.y && z >= rhs.z;
        }

        bool operator<=(const Vec3& rhs) const
        {
            return x <= rhs.x && y <= rhs.y && z <= rhs.z;
        }

        bool operator==(const Vec3& rhs) const
        {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }
    };

    static double dot(const Vec3& lhs, const Vec3& rhs)
    {
        return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
    }

    static Vec3 cross(const Vec3& a, const Vec3& b)
    {
        return {
            a.y * b.z - a.z * b.y,
            a.z * b.x - a.x * b.z,
            a.x * b.y - a.y * b.x
        };
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

        bool operator==(const Vec2& rhs) const
        {
            return x == rhs.x && y == rhs.y;
        }
    };

    inline bool LineLineIntersection(const Vec3& p0, const Vec3& d0, const Vec3& p1, const Vec3& d1, Vec3& poi) {
        Vec3 n = cross(d0, d1); // Normal to both direction vectors
        double normN = dot(n, n);  // Magnitude squared of the normal vector
        if (std::fabs(normN) < 1e-9) return false; // Lines are parallel and do not intersect

        //https://en.wikipedia.org/wiki/Line%E2%80%93line_intersection
        double x1 = p0.x;
        double y1 = p0.y;
        double x2 = p0.x + d0.x;
        double y2 = p0.y + d0.y;
        double x3 = p1.x;
        double y3 = p1.y;
        double x4 = p1.x + d1.x;
        double y4 = p1.y + d1.y;

        double t = ((x1-x3)*(y3-y4)-(y1-y3)*(x3-x4))/((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));
        double s = -((x1-x2)*(y1-y3)-(y1-y2)*(x1-x3))/((x1-x2)*(y3-y4)-(y1-y2)*(x3-x4));

        Vec3 poi0 = p0 + (d0 * t);
        Vec3 poi1 = p1 + (d1 * s);

        poi = poi0 + (poi1 - poi0) * 0.5;

        if((poi0 - poi1).length2() < 1e-9) return true; // Points are to far appart

        return false;
    }

} // namespace MLib


#endif //MLIB_HPP