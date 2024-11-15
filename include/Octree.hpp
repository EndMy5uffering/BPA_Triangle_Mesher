#ifndef OCTREE_HPP
#define OCTREE_HPP

#include <vector>
#include <memory>
#include <Geometry.hpp>
#include <Collider.hpp>
#include <iostream>

struct Node{
    MLib::Vec3* mData = nullptr;
    bool mIsLeaf = false;
    Collider::AABB mBounds;
    std::unique_ptr<Node> mSubNodes[8];

    bool insert(MLib::Vec3* vert);
    void search(Collider::Sphere& s, std::vector<MLib::Vec3*>& verts);

    void print(int depth);

    unsigned long depthCountValues()
    {
        if(mIsLeaf) return mData != nullptr ? 1 : 0;

        int leafCount = 0;
        for(int i = 0; i < 8; ++i)
        {
            leafCount += mSubNodes[i]->depthCountValues();
        }

        return leafCount + (mData != nullptr ? 1 : 0);
    }

    unsigned long depthCountNodes()
    {
        if(mIsLeaf) return 1;

        int leafCount = 0;
        for(int i = 0; i < 8; ++i)
        {
            leafCount += mSubNodes[i]->depthCountNodes();
        }

        return leafCount + 1;
    }
};

class Octree{

public:

    Octree(Collider::AABB bb) : 
    mBounds{bb}
    {
        mRoot = std::make_unique<Node>();
        mRoot->mBounds = bb;
        mRoot->mIsLeaf = true;
    }

    bool insert(MLib::Vec3* data);
    void search(Collider::Sphere &s, std::vector<MLib::Vec3*>& verts);

    void print();

    unsigned long depthCountValues()
    {
        return mRoot->depthCountValues();
    }

    unsigned long depthCountNodes()
    {
        return mRoot->depthCountNodes();
    }

private:

    Collider::AABB mBounds;

    std::unique_ptr<Node> mRoot;

};



#endif //OCTREE_HPP

