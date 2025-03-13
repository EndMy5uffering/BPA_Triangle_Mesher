#ifndef GRID_HPP
#define GRID_HPP

#include<list>
#include<vector>
#include<Geometry.hpp>
#include<gtx/norm.hpp>
#include<BugTools.hpp>

//from
//https://github.com/bernhardmgruber/bpa/blob/master/src/lib/bpa.cpp

using Cell = std::vector<Geometry::Vertex*>;

class Grid {

public:
    Grid(std::vector<Geometry::Vertex>& verts, float radius) : cellSize(radius * 2), vert_list{verts} {
        lower = verts[0].position;
        upper = verts[0].position;

        for (const Geometry::Vertex& p : verts) {
            for (auto i = 0; i < 3; i++) {
                lower[i] = std::min(lower[i], p.position[i]);
                upper[i] = std::max(upper[i], p.position[i]);
            }
        }

        dims = glm::max(glm::ivec3{ceil((upper - lower) / cellSize)}, glm::ivec3{1});
        
        cells.resize(dims.x * dims.y * dims.z);
        for (size_t i = 0; i < verts.size(); ++i)
            cell(cellIndex(verts[i].position)).push_back(&verts[i]);
    }

    glm::ivec3 cellIndex(glm::vec3 point) {
        const auto index = glm::ivec3{(point - lower) / cellSize};
        return clamp(index, glm::ivec3{}, dims - 1);
    }

    Cell& cell(glm::ivec3 index) 
    { 
        return cells[index.z * dims.x * dims.y + index.y * dims.x + index.x]; 
    }

    bool isPointWithinCellWithMargin(const glm::vec3& point, const glm::ivec3& cellIndex, float margin) {
        glm::vec3 cellLower = lower + glm::vec3(cellIndex) * cellSize;
        glm::vec3 cellUpper = cellLower + glm::vec3(cellSize);
    
        glm::vec3 minBound = cellLower - glm::vec3(margin);
        glm::vec3 maxBound = cellUpper + glm::vec3(margin);
    
        return (point.x >= minBound.x && point.x <= maxBound.x) &&
               (point.y >= minBound.y && point.y <= maxBound.y) &&
               (point.z >= minBound.z && point.z <= maxBound.z);
    }

    Cell sphericalNeighborhood(glm::vec3 point, std::initializer_list<glm::vec3> ignore) {
        std::vector<Geometry::Vertex*> result;
        glm::ivec3 centerIndex = cellIndex(point);
        result.reserve(cell(centerIndex).size() * 27); // just an estimate
        for (auto xOff : {-1, 0, 1}) {
            for (auto yOff : {-1, 0, 1}) {
                for (auto zOff : {-1, 0, 1}) {
                    glm::ivec3 index = centerIndex + glm::ivec3{xOff, yOff, zOff};
                    if (index.x < 0 || index.x >= dims.x)
                        continue;
                    if (index.y < 0 || index.y >= dims.y)
                        continue;
                    if (index.z < 0 || index.z >= dims.z)
                        continue;
                    std::vector<Geometry::Vertex*>& c = cell(index);
                    for (Geometry::Vertex* vert : c)
                        if (glm::length2(vert->position - point) < cellSize * cellSize && std::find(begin(ignore), end(ignore), vert->position) == end(ignore))
                            result.push_back(vert);
                }
            }
        }
        return result;
    }

    Cell sphericalNeighborhoodWithMargin(glm::vec3 point, float margin, std::initializer_list<glm::vec3> ignore) {
        Cell result;
        glm::ivec3 centerIndex = cellIndex(point);
        float searchRadius = cellSize + margin;
        float searchRadiusSquared = searchRadius * searchRadius;
        
        result.reserve(cell(centerIndex).size() * 27); // just an estimate
        
        for (auto xOff : {-1, 0, 1}) {
            for (auto yOff : {-1, 0, 1}) {
                for (auto zOff : {-1, 0, 1}) {
                    glm::ivec3 index = centerIndex + glm::ivec3{xOff, yOff, zOff};
                    if (index.x < 0 || index.x >= dims.x)
                        continue;
                    if (index.y < 0 || index.y >= dims.y)
                        continue;
                    if (index.z < 0 || index.z >= dims.z)
                        continue;
                    
                    Cell& c = cell(index);
                    for (Geometry::Vertex* vert : c)
                        if (glm::length2(vert->position - point) < searchRadiusSquared && 
                            std::find(begin(ignore), end(ignore), vert->position) == end(ignore)) {
                            result.push_back(vert);
                        }
                }
            }
        }
        
        return result;
    }

    std::array<std::vector<glm::ivec3>, 8> getThreadCellList()
    {
        std::array<std::vector<glm::ivec3>, 8> result;

        for(int z = 0; z < dims.z; z += 2)
        {
            for(int y = 0; y < dims.y; y += 2)
            {
                for(int x = 0; x < dims.x; x += 2)
                {
                    //Skipping empty cells
                    if(cell({x,y,z}).size() > 0) result[0].push_back({x,y,z});
                    if(x + 1 < dims.x && cell({x + 1,y,z}).size() > 0) result[1].push_back({x + 1,y,z});
                    if(y + 1 < dims.y && cell({x,y + 1,z}).size() > 0) result[2].push_back({x,y + 1,z});
                    if(z + 1 < dims.z && cell({x,y,z + 1}).size() > 0) result[3].push_back({x,y,z + 1});
                    if(x + 1 < dims.x && y + 1 < dims.y && cell({x + 1,y + 1,z}).size() > 0) result[4].push_back({x + 1,y + 1,z});
                    if(x + 1 < dims.x && z + 1 < dims.z && cell({x + 1,y,z + 1}).size() > 0) result[5].push_back({x + 1,y,z + 1});
                    if(y + 1 < dims.y && z + 1 < dims.z && cell({x,y + 1,z + 1}).size() > 0) result[6].push_back({x,y + 1,z + 1});
                    if(x + 1 < dims.x && y + 1 < dims.y && z + 1 < dims.z && cell({x + 1,y + 1,z + 1}).size() > 0) result[7].push_back({x + 1,y + 1,z + 1});

                }
            }
        }

        return result;
    }

    std::vector<Geometry::Vertex>& getVertList()
    {
        return vert_list;
    }

    std::vector<Cell>& getCells()
    {
        return cells;
    }

    glm::ivec3 getDims()
    {
        return dims;
    }

private:
    glm::vec3 lower;
    glm::vec3 upper;
    float cellSize;
    glm::ivec3 dims;
    std::vector<Cell> cells;

    std::vector<Geometry::Vertex>& vert_list;
};

#endif //GRID_HPP