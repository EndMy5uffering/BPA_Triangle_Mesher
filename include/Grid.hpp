#ifndef GRID_HPP
#define GRID_HPP

#include<list>
#include<vector>
#include<Geometry.hpp>

class Grid
{

public:

    Grid(double width, double height, double depth, double dx, double dy, double dz);

    std::list<Geometry::Vertex*>& at(double x, double y, double z);
    

private:
    double width, height, depth;
    double dx, dy, dz;

    std::vector<std::list<Geometry::Vertex*>> data;

};

#endif //GRID_HPP