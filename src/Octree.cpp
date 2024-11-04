#include "Octree.hpp"

#define CubePosIDX(n, x) ((-1 * ((x >> n) & 1)) + (1-((x >> n) & 1)))

bool Octree::insert(Vertex *data)
{
    Node* root = mRoot.get();

    if(root == nullptr) return false; // Root should not be null
    if(data == nullptr) return false;

    return root->insert(data);
}

bool Octree::search(double x, double y, double z, double w, double h, double d, std::vector<Vertex*> &data)
{
    return false;
}

bool Octree::search(double x, double y, double z, double dist, std::vector<Vertex*> &verts)
{
    return false;
}

void Octree::print()
{
    if(mRoot == nullptr) return;

    mRoot->print(0);
}

bool Node::insert(Vertex *vert)
{
    if(!Box::contains(mBounds, vert->position)) return false;
    
    if(mData == nullptr) 
    {
        mData = vert;
        return true;
    }

    if(!mIsLeaf && mData != nullptr)
    {
        for(int i = 0; i < 8; ++i)
        {
            if(Box::contains(mSubNodes[i]->mBounds, vert->position))
            {
                return mSubNodes[i]->insert(vert);
            }
        }
    }

    if(mIsLeaf && mData != nullptr)
    {
        mIsLeaf = false;
        MLib::Vec3 nSize = mBounds.size / 2.0;
        MLib::Vec3 off = mBounds.size / 4.0;
        MLib::Vec3 p = mBounds.position;
        for(int i = 0; i < 8; ++i)
        {
            Box::AABB nBound{{p.x + (off.x * CubePosIDX(0, i)), p.y + (off.y * CubePosIDX(1, i)), p.z + (off.z * CubePosIDX(2, i))}, nSize};
            mSubNodes[i] = std::make_unique<Node>();
            mSubNodes[i]->mBounds = nBound;
            mSubNodes[i]->mIsLeaf = true;
        }

        for(int i = 0; i < 8; ++i)
        {
            if(Box::contains(mSubNodes[i]->mBounds, vert->position))
            {
                return mSubNodes[i]->insert(vert);
            }
        }
    }

    return false;
}

void Node::print(int depth)
{

    for(int i = 0; i < depth; ++i) std::cout << "-";
    std::cout << "Pos( " << mBounds.position.x << ", " << mBounds.position.y << ", " << mBounds.position.z 
    << " ) Size( " << mBounds.size.x << ", " << mBounds.size.y << ", " << mBounds.size.z << ") IsLeaf:" << mIsLeaf << " HasData:" << (mData != nullptr) << "\n";

    if(!mIsLeaf)
    {
        for(int i = 0; i < 8; ++i)
        {
            mSubNodes[i]->print(depth + 1);
        }
    }
}
