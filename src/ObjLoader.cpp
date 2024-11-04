#include "ObjLoader.hpp"

double parseDouble(std::ifstream& stream)
{
    std::string accumulator;
    char c;
    while(stream.get(c))
    {
        if(std::isdigit(c)) accumulator += c;
        if(c == '.') accumulator += c;
        char next = stream.peek();
        if(next == ' ' || next == '\n' || next == 0) 
        {
            stream.get(c);
            break;
        }
    }
    return std::stod(accumulator);
}

int parseInt(std::ifstream& stream)
{
    std::string accumulator;
    char c;
    while(stream.get(c))
    {
        if(std::isdigit(c)) 
        {
            accumulator += c;
        }
        else
        {
            std::cerr << "Can not parse string literal as number: " << c << "\n";
            return 0;
        }
        char next = stream.peek();
        if(next == ' ' || next == '\n' || next == 0) 
        {
            stream.get(c);
            break;
        }
    }
    return std::stoi(accumulator);
}

int parseIntFaceData(std::ifstream& stream)
{
    
    std::string accumulator;
    char c;
    if(stream.peek() == '/') // early breakout if face = f v1//vn1 <missing vertex texture>
    {
        stream.get(c);
        return -1;
    }
    while(stream.get(c))
    {
        char next = stream.peek();
        if(next == ' ' || next == '\n' || next == 0 || next == '/') 
        {
            break;
        }
        if(std::isdigit(c)) 
        {
            accumulator += c;
        }
        else
        {
            std::cerr << "Can not parse string literal as number: " << c << "\n";
            return 0;
        }
    }
    return std::stoi(accumulator);
}

MLib::Vec3 parseVertexPosition(std::ifstream& stream)
{
    double x = parseDouble(stream);
    double y = parseDouble(stream);
    double z = parseDouble(stream);

    if(stream.peek() == ' ') parseDouble(stream); // potential w value for a vertex

    return MLib::Vec3{x,y,z};
}

MLib::Vec3 parseVertexNormal(std::ifstream& stream)
{
    double x = parseDouble(stream);
    double y = parseDouble(stream);
    double z = parseDouble(stream);

    return MLib::Vec3{x,y,z};
}

OBJ::FaceData parseFaceData(std::ifstream& stream)
{
    OBJ::FaceData face;
    char c;
    int state = 0;
    while(stream.get(c) && c != '\n' && c == ' ')
    {
        int v = parseIntFaceData(stream);
        if(stream.peek() == '/') stream.get(c);
        int vt = parseIntFaceData(stream);
        if(stream.peek() == '/') stream.get(c);
        int vn = parseIntFaceData(stream);
        if(stream.peek() != ' ')
        {
            std::cerr << "Unexpected symbol!\n";
            return OBJ::FaceData{};
        }
        stream.get(c);

        face.vertexIDXList.push_back(v);
        face.vertexTextureIDXList.push_back(vt);
        face.vertexNormalIDXList.push_back(vn);
    }
    return face;
}

void OBJ::load(const char *path)
{
    std::ifstream file(path); // Open the file

    if (!file) {
        std::cerr << "Error: Could not open the file " << path << std::endl;
        return;
    }

    char c;
    while (file.get(c)) { // Read line by line
        if(c == 0) break;
        if(c == '\n') continue;
        if(c == 'v' && file.peek() == ' ') 
        {
            file.get(c);
            MLib::Vec3 v = parseVertexPosition(file);
            std::cout << "Poisition: x " << v.x << " y " << v.y << " z " << v.z << "\n";
        }
        if(c == 'v' && file.peek() == 'n')
        {
            file.get(c);
            file.get(c);
            MLib::Vec3 vn = parseVertexNormal(file);
            std::cout << "Normal: x " << vn.x << " y " << vn.y << " z " << vn.z << "\n";
        }
        if(c == 'f' && file.peek() == ' ')
        {
            OBJ::FaceData face = parseFaceData(file);
            std::cout << "Face: ";
            for(int i = 0 ; i < face.vertexIDXList.size(); ++i)
            {
                std::cout << face.vertexIDXList[i] << "/" << face.vertexTextureIDXList[i] << "/"<< face.vertexNormalIDXList[i] << " ";
            }
            std::cout << "\n";
        }
    }

    file.close(); // Close the file
}

void OBJ::save()
{

}
