#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <tuple>
#include <map>
#include <glm.hpp>
#include <Geometry.hpp>
#include <PBA.hpp>

namespace OBJ
{

    struct Face {
        std::vector<int> vertexIndices;  
        std::vector<int> textureIndices; 
        std::vector<int> normalIndices;  

        bool hasTextureIndeces()
        {
            return textureIndices.size() > 0;
        }

        bool hasNormalIndices()
        {
            return normalIndices.size() > 0;
        }
    };

    struct Model {
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> textures;
        std::vector<glm::vec3> normals;
        std::vector<Face> faces;
    };

    inline Model parse(const std::string& filename) {
        Model model;
        std::ifstream file(filename);

        if (!file.is_open()) {
            //throw std::runtime_error("Failed to open file: " + filename);
            std::cerr << "Failed to open file: " << filename << "\n";
            return model;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream lineStream(line);
            std::string prefix;
            lineStream >> prefix;

            if (prefix == "v") {  // Vertex line
                glm::vec3 vertex;
                lineStream >> vertex.x >> vertex.y >> vertex.z;
                model.vertices.push_back(vertex);
            } else if (prefix == "vt") {  // Texture coordinate line
                glm::vec2 texture;
                lineStream >> texture.x >> texture.y;
                model.textures.push_back(texture);
            } else if (prefix == "vn") {  // Normal line
                glm::vec3 normal;
                lineStream >> normal.x >> normal.y >> normal.z;
                model.normals.push_back(normal);
            } else if (prefix == "f") {  // Face line
                Face face;
                std::string vertexData;

                while (lineStream >> vertexData) {
                    std::istringstream vertexStream(vertexData);
                    std::string indexString;
                    int vertexIndex, textureIndex, normalIndex;

                    if (std::getline(vertexStream, indexString, '/')) {
                        vertexIndex = std::stoi(indexString) - 1;
                        face.vertexIndices.push_back(vertexIndex);
                    }

                    if (std::getline(vertexStream, indexString, '/')) {
                        if (!indexString.empty()) {
                            textureIndex = std::stoi(indexString) - 1;
                            face.textureIndices.push_back(textureIndex);
                        }
                    }

                    if (std::getline(vertexStream, indexString)) {
                        normalIndex = std::stoi(indexString) - 1;
                        face.normalIndices.push_back(normalIndex);
                    }
                }

                model.faces.push_back(face);
            }
        }

        file.close();
        return model;
    }

    /**
     * @brief This function could crash if normals or vertices are missing or the indeces are pointing to a wrong location
     * 
     * @param mesh 
     * @return std::vector<Geometry::Vertex> 
     */
    inline std::vector<Geometry::Vertex> ModelToVertexList(Model& mesh)
    {
        std::vector<Geometry::Vertex> verts;
        verts.resize(mesh.vertices.size());

        for(Face& f : mesh.faces)
        {
            for(int i = 0; i < 3; ++i)
            {
                glm::vec3 v = mesh.vertices[f.vertexIndices[i]];
                glm::vec3 n = mesh.normals[f.normalIndices[i]];
                
                verts[f.vertexIndices[i]].position = v;
                verts[f.vertexIndices[i]].normal = glm::normalize((0.5f * (verts[f.vertexIndices[i]].normal + n)));

            }
        }
        return verts;
    }

    static void SaveObject(const std::string& filename, Geometry::Mesh& mesh, PBA::VertList& vlist)
    {
        std::ofstream file(filename);

        if (!file.is_open()) {
            //throw std::runtime_error("Failed to open file: " + filename);
            std::cerr << "Failed to open file: " << filename << "\n";
            return;
        }

        for(size_t i = 0; i < vlist.size(); ++i)
        {
            file << "v " << vlist[i].position.x << " " << vlist[i].position.y << " " << vlist[i].position.z << "\n";
        }
        for(size_t i = 0; i < vlist.size(); ++i)
        {
            file << "vn " << vlist[i].normal.x << " " << vlist[i].normal.y << " " << vlist[i].position.z << "\n";
        }

        Geometry::Vertex* start = &vlist[0];

        for(auto it = mesh.m_triangles.begin(); it != mesh.m_triangles.end(); ++it)
        {
            //if(!it->can_export) continue;
            file << "f " 
            << ((it->at(0) - start) + 1) << "//" << ((it->at(0) - start) + 1) << " " 
            << ((it->at(1) - start) + 1) << "//" << ((it->at(1) - start) + 1) << " " 
            << ((it->at(2) - start) + 1) << "//" << ((it->at(2) - start) + 1) << "\n";
        }

        file.close();

    }

}


#endif //OBJLOADER_HPP