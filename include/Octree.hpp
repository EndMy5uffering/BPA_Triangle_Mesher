#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <vector>
#include <memory>
#include <Geometry.hpp>
#include <Box.hpp>
#include <iostream>


struct BoundingBox{
    double mX;
    double mY;
    double mZ;
    double mW;
    double mH;
    double mD;
};

struct Node{
    Vertex* mData = nullptr;
    bool mIsLeaf = false;
    Box::AABB mBounds;
    std::unique_ptr<Node> mSubNodes[8];

    bool insert(Vertex* vert);
    bool search(double x, double y, double z, double w, double h, double d, std::vector<Vertex*>& data);
    bool search(double x, double y, double z, double dist, std::vector<Vertex*>& verts);

    void print(int depth);
};

class Octree{

public:

    Octree(Box::AABB bb) : 
    mBounds{bb}
    {
        mRoot = std::make_unique<Node>();
        mRoot->mBounds = bb;
        mRoot->mIsLeaf = true;
    }

    bool insert(Vertex* data);
    bool search(double x, double y, double z, double w, double h, double d, std::vector<Vertex*>& data);
    bool search(double x, double y, double z, double dist, std::vector<Vertex*>& verts);

    void print();

private:

    Box::AABB mBounds;

    std::unique_ptr<Node> mRoot;

};



#endif //OCTREE_HPP


