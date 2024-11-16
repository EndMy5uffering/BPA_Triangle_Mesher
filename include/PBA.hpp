#ifndef PBA_HPP
#define PBA_HPP

#include <Collider.hpp>
#include <Geometry.hpp>
#include <MLib.hpp>

namespace PBA
{

    static bool IsSeedTriangle(MLib::Vec3 a, MLib::Vec3 b, MLib::Vec3 c, float p)
    {

        MLib::Vec3 p_1 = c + (a - c) * 0.5;
        MLib::Vec3 p_2 = c + (b - c) * 0.5;

        MLib::Vec3 n = MLib::cross((a-c),(b-c)).norm();

        MLib::Vec3 ca = a - c;
        MLib::Vec3 cao{-ca.y, ca.x, ca.z};
        cao = (cao - n*(MLib::dot(cao, n)/MLib::dot(n,n)) - ca*(MLib::dot(cao,ca)/MLib::dot(ca,ca))).norm();
        

        MLib::Vec3 cb = b - c;
        MLib::Vec3 cbo{-cb.y, cb.x, cb.z};
        cbo = (cbo - n*(MLib::dot(cbo, n)/MLib::dot(n,n)) - cb*(MLib::dot(cbo,cb)/MLib::dot(cb,cb))).norm();

        MLib::Vec3 poi;
        if(!MLib::LineLineIntersection(p_1, cao, p_2, cbo, poi)) return false;

        if((poi - a).length2() > p) return false;

        return true;
    }   

}

#endif //PBA_HPP