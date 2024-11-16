//PROJECT WAS CREATED WITH PYMAKE
//PYMAKE: https://github.com/EndMy5uffering/PyCmakeToolKit
//THIS FILE CAN BE CHANGED TO FIT YOUR PROJECT. THIS IS ONLY A TEMPLATE FOR A QUICK SETUP.

#include <chrono>

#include <Octree.hpp>
#include <ObjLoader.hpp>

#include<PBA.hpp>

#define MEASURE_EXECUTION_TIME(code_block, time_pre_text)                   \
    {                                                                       \
        auto start = std::chrono::high_resolution_clock::now();             \
        code_block                                                          \
        auto end = std::chrono::high_resolution_clock::now();               \
        auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start); \
        std::cout << time_pre_text << duration.count() << "ns" << std::endl; \
    }

void OBJLoaderTest()
{
    OBJ::Model data;

    MEASURE_EXECUTION_TIME({data = OBJ::parse("D:\\teapot.obj");}, "OBJ Loading Time: ")

    Octree tree{{{0,0,0}, {20,20,20}}};
    MEASURE_EXECUTION_TIME({
        for(size_t i = 0; i < data.vertices.size(); ++i)
        {
            if(!tree.insert(&data.vertices[i])) std::cerr << "Did not insert point at ( " << data.vertices[i].x << ", " << data.vertices[i].y << ", " << data.vertices[i].z << " )\n";
        }  
    }, "Tree insertion time: ")
    std::cout << "Inserted " << data.vertices.size() << " vertices\n";
    std::cout << "Values in tree found: " << tree.depthCountValues() << "\n";
    std::cout << "Nodes in tree found: " << tree.depthCountNodes() << "\n";


    std::vector<MLib::Vec3*> verts;
    Collider::Sphere s{1,1,1,2};
    MEASURE_EXECUTION_TIME({tree.search(s, verts);}, "Tree Search: ")
    std::cout << "Found verts: " << verts.size() << "\n";

    std::vector<MLib::Vec3*> verts2;
    MEASURE_EXECUTION_TIME({
        for(size_t i = 0; i < data.vertices.size(); ++i)
        {
            if(s >= data.vertices[i]) verts2.push_back(&data.vertices[i]);
        }
    }, "Stupid search: ")
    

    std::cout << "Found verts2: " << verts2.size() << "\n";

    bool stupidsearch = (verts2.size() == verts.size());
    MEASURE_EXECUTION_TIME({
        for(MLib::Vec3* v : verts)
        {
            bool found = false;
            for(MLib::Vec3* v2 : verts2)
            {
                if(*v2 == *v) found = true;
            }
            stupidsearch &= found;
        }
    },"Lazy compair: ")

    std::cout << "Vectors are same: " << stupidsearch << "\n";
}


int main(void) 
{
    //OBJLoaderTest();


    MLib::Vec3 A{1,1,0};
    MLib::Vec3 B{-2,2,1};
    MLib::Vec3 C{-1,-1,0};

    //std::cout << "Is seed triangle: " << PBA::IsSeedTriangle(A,B,C,2.6f) << "\n";

    MLib::Vec3 p0{1,1,1};
    MLib::Vec3 p1{-1,0,3};
    MLib::Vec3 d0{-1,2,1};
    MLib::Vec3 d1{-1.5,-2,1.5};

    MLib::Vec3 poi{0,0,0};

    bool found = MLib::LineLineIntersection(p0,d0,p1,d1,poi);

    std::cout << "Found: " << found << " at: {" <<  poi.x << "; " << poi.y << "; " << poi.z << "}\n";
     
}

