#include "Octree.hpp"

#define CubePosIDX(n, x) ((-1 * ((x >> n) & 1)) + (1-((x >> n) & 1)))

bool Octree::insert(MLib::Vec3 *point)
{
    Node* root = mRoot.get();

    if(root == nullptr) return false; // Root should not be null

    return root->insert(point);
}

void Octree::search(Collider::Sphere& s, std::vector<MLib::Vec3*> &verts)
{
    Node* root = mRoot.get();

    if(root == nullptr) return; // Root should not be null

    root->search(s, verts);
}

void Octree::print()
{
    if(mRoot == nullptr) return;

    mRoot->print(0);
}

bool Node::insert(MLib::Vec3 *point)
{
    if(mBounds < *point) return false;
    
    if(mData == nullptr) 
    {
        mData = point;
        return true;
    }

    if(!mIsLeaf && mData != nullptr)
    {
        for(int i = 0; i < 8; ++i)
        {
            if(mSubNodes[i]->mBounds >= *point)
            {
                return mSubNodes[i]->insert(point);
            }
        }
        std::cerr << "[Octree]: Could not insert vertex ( " << point->x << "; " << point->y << "; " << point->z << " )\n";
        return false;
    }

    if(mIsLeaf && mData != nullptr)
    {
        mIsLeaf = false;
        MLib::Vec3 nSize = mBounds.size / 2.0;
        MLib::Vec3 off = mBounds.size / 4.0;
        MLib::Vec3 bp = mBounds.position;
        for(int i = 0; i < 8; ++i)
        {
            Collider::AABB nBound{{bp.x + (off.x * CubePosIDX(0, i)), bp.y + (off.y * CubePosIDX(1, i)), bp.z + (off.z * CubePosIDX(2, i))}, nSize};
            mSubNodes[i] = std::make_unique<Node>();
            mSubNodes[i]->mBounds = nBound;
            mSubNodes[i]->mIsLeaf = true;
        }

        for(int i = 0; i < 8; ++i)
        {
            if(mSubNodes[i]->mBounds >= *point)
            {
                return mSubNodes[i]->insert(point);
            }
        }
        std::cerr << "[Octree]: Could not insert vertex ( " << point->x << "; " << point->y << "; " << point->z << " )\n";
        return false;
    }

    return false;
}

void Node::search(Collider::Sphere &s, std::vector<MLib::Vec3 *> &verts)
{
    if(mData != nullptr && s >= *mData) verts.push_back(mData);

    if(!mIsLeaf)
    {
        for (int i = 0; i < 8; ++i) 
        {
            if (mSubNodes[i] && (mSubNodes[i]->mBounds & s)) 
            {
                mSubNodes[i]->search(s, verts);
            }
        }
    }

    return;
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