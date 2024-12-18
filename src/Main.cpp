//PROJECT WAS CREATED WITH PYMAKE
//PYMAKE: https://github.com/EndMy5uffering/PyCmakeToolKit
//THIS FILE CAN BE CHANGED TO FIT YOUR PROJECT. THIS IS ONLY A TEMPLATE FOR A QUICK SETUP.

#include <chrono>

#include <Octree.hpp>
#include <ObjLoader.hpp>

#include <list>

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

    MEASURE_EXECUTION_TIME({data = OBJ::parse("D:\\Bibliotheken\\Desktop\\MeshingOutput\\Input\\Bunny.obj");}, "OBJ Loading Time: ")

    MEASURE_EXECUTION_TIME({OBJ::ModelToVertexList(data);}, "Time to tranform mesh to vert list: ");

    PBA::VertList vlist = OBJ::ModelToVertexList(data);

    PBA::Mesh m{std::move(PBA::PivotBall(vlist, 0.02))};
    
    std::cout << "Done meshing\n"; 
    std::cout << "Saving file\n"; 

    OBJ::SaveObject("D:\\Bibliotheken\\Desktop\\MeshingOutput\\BunnyOutput.obj", m, vlist);
    std::cout << "Done done\n"; 

}

void test2()
{
    MLib::Vec3 A{1,1,0};
    MLib::Vec3 B{-2,2,1};
    MLib::Vec3 C{-1,-1,0};
    MLib::Vec3 D{1,-2,-1};
    MLib::Vec3 n1{0.24, -0.24, 0.94};
    MLib::Vec3 n2{0.3, -0.3, 0.9};

    PBA::VertList list;
    list.emplace_back(A, n1, MLib::Vec2{0,0});
    list.emplace_back(B, n1, MLib::Vec2{0,0});
    list.emplace_back(C, n1, MLib::Vec2{0,0});
    list.emplace_back(D, n2, MLib::Vec2{0,0});

    PBA::Mesh m{std::move(PBA::PivotBall(list, 3.0))};
    std::cout << "Done meshing\n"; 

    OBJ::SaveObject("D:\\Bibliotheken\\Desktop\\MeshingOutput\\OBJOUTPUT_TestFunc.obj", m, list);
}

int main(void) 
{
    //std::ofstream out("LOG.txt");
    //std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    //std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

    OBJLoaderTest();

    //test2();


    std::cout << "Done...\n"; 
}

