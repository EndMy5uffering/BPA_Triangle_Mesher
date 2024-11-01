#ifndef MUTIL_HPP
#define MUTIL_HPP

namespace MUtil
{

    template<typename T>
    T min(T a, T b)
    {
        return a < b ? a : b;
    }

    template<typename T>
    T max(T a, T b)
    {
        return a > b ? a : b;
    }

    struct Vec3{
        double x;
        double y;
        double z;

        Vec3 operator+(const Vec3& rhs)
        {
            return Vec3{x + rhs.x, y + rhs.y, z + rhs.z};
        }

        Vec3 operator-(const Vec3& rhs)
        {
            return Vec3{x - rhs.x, y - rhs.y, z - rhs.z};
        }

        template<typename T>
        Vec3 operator/(T rhs)
        {
            return Vec3{x / rhs, y / rhs, z / rhs};
        }

        template<typename T>
        Vec3& operator/=(T rhs)
        {
            x /= rhs;
            y /= rhs; 
            z /= rhs;
            return *this;
        }

        template<typename T>
        Vec3 operator*(T rhs)
        {
            return Vec3{x * rhs, y * rhs, z * rhs};
        }

        template<typename T>
        Vec3& operator*=(T rhs)
        {
            x *= rhs;
            y *= rhs; 
            z *= rhs;
            return *this;
        }

        bool operator<(const Vec3& rhs)
        {
            return x < rhs.x && y < rhs.y && z < rhs.z;
        }

        bool operator>(const Vec3& rhs)
        {
            return x > rhs.x && y > rhs.y && z > rhs.z;
        }

        bool operator>=(const Vec3& rhs)
        {
            return x >= rhs.x && y >= rhs.y && z >= rhs.z;
        }

        bool operator<=(const Vec3& rhs)
        {
            return x <= rhs.x && y <= rhs.y && z <= rhs.z;
        }

        bool operator==(const Vec3& rhs)
        {
            return x == rhs.x && y == rhs.y && z == rhs.z;
        }
    };

} // namespace MUtil


#endif //MUTIL_HPP