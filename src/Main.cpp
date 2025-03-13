//PROJECT WAS CREATED WITH PYMAKE
//PYMAKE: https://github.com/EndMy5uffering/PyCmakeToolKit
//THIS FILE CAN BE CHANGED TO FIT YOUR PROJECT. THIS IS ONLY A TEMPLATE FOR A QUICK SETUP.

#define DEBUG

#include <chrono>

#include <list>

#include<PBA.hpp>
#include <PBAMT.hpp>

#include <glm.hpp>
#include <ObjLoader.hpp>

// Declare the kernel launcher defined in kernel.cu
//extern "C" void launchKernel();

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

    Geometry::Mesh m{std::move(PBA::PivotBall(vlist, 0.025f))};
    
    std::cout << "Done meshing\n"; 
    std::cout << "Saving file\n"; 

    OBJ::SaveObject("D:\\Bibliotheken\\Desktop\\MeshingOutput\\BunnyOut.obj", m, vlist);
    std::cout << "Done done\n"; 

}

void test2()
{
    glm::vec3 A{1,1,0};
    glm::vec3 B{-2,2,1};
    glm::vec3 C{-1,-1,0};
    glm::vec3 D{1,-2,-1};
    glm::vec3 n1{0.24, -0.24, 0.94};
    glm::vec3 n2{0.3, -0.3, 0.9};

    glm::vec2 uv{0,0};

    PBA::VertList list;
    list.push_back({A, n1, uv, false});
    list.push_back({B, n1, uv, false});
    list.push_back({C, n1, uv, false});
    list.push_back({D, n2, uv, false});

    Geometry::Mesh m{std::move(PBA::PivotBall(list, 3.0))};
    std::cout << "Done meshing\n"; 

    OBJ::SaveObject("D:\\Bibliotheken\\Desktop\\MeshingOutput\\OBJOUTPUT_TestFunc.obj", m, list);
}

void threadedTest()
{
    OBJ::Model data;

    MEASURE_EXECUTION_TIME({data = OBJ::parse("D:\\Bibliotheken\\Desktop\\MeshingOutput\\Input\\Bunny.obj");}, "OBJ Loading Time: ")

    auto [minPoint, maxPoint, dimensions] = OBJ::computeMinMaxDim(data);

    MEASURE_EXECUTION_TIME({OBJ::ModelToVertexList(data);}, "Time to tranform mesh to vert list: ");

    PBA::VertList vlist = OBJ::ModelToVertexList(data);

    float len = glm::max(dimensions.x, glm::max(dimensions.y, dimensions.z)) / 30.0f;
    Geometry::Mesh m{std::move(PBAMT::PivotBall(vlist, len, len * 0.8f, 6))};
    
    std::cout << "Done meshing\n"; 
    std::cout << "Saving file\n"; 

    OBJ::SaveObject("D:\\Bibliotheken\\Desktop\\MeshingOutput\\BunnyOut.obj", m, vlist);
    
    std::cout << "File Written to: D:\\Bibliotheken\\Desktop\\MeshingOutput\\BunnyOut.obj\n"; 
    std::cout << "Done done\n";
}

int main(void) 
{
    //std::ofstream out("LOG.txt");
    //std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
    //std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!

    threadedTest();

    //test2();

    //std::cout << "Launching CUDA Kernel..." << std::endl;
    //launchKernel();
    //std::cout << "Done...\n"; 
}

