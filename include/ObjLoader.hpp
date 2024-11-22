#ifndef OBJLOADER_HPP
#define OBJLOADER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <Geometry.hpp>

#include <MLib.hpp>

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
        std::vector<MLib::Vec3> vertices;
        std::vector<MLib::Vec2> textures;
        std::vector<MLib::Vec3> normals;
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
                MLib::Vec3 vertex;
                lineStream >> vertex.x >> vertex.y >> vertex.z;
                model.vertices.push_back(vertex);
            } else if (prefix == "vt") {  // Texture coordinate line
                MLib::Vec2 texture;
                lineStream >> texture.x >> texture.y;
                model.textures.push_back(texture);
            } else if (prefix == "vn") {  // Normal line
                MLib::Vec3 normal;
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
        verts.resize(mesh.vertices.size(), {{0.0, 0.0, 0.0}, {0.0, 0.0, 0.0}, {0.0, 0.0}});

        for(Face& f : mesh.faces)
        {
            for(int i = 0; i < 3; ++i)
            {
                MLib::Vec3 v = mesh.vertices[f.vertexIndices[i]];
                MLib::Vec3 n = mesh.normals[f.normalIndices[i]];
                
                verts[f.vertexIndices[i]].position = v;
                verts[f.vertexIndices[i]].normal = ((verts[f.vertexIndices[i]].normal + n) * 0.5).norm();

            }
        }
        return verts;
    }

}


#endif //OBJLOADER_HPP