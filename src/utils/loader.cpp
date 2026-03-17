#include "loader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

// trim whitespace from both ends
namespace {

std::string trim(const std::string& s) {
    size_t start = 0;
    while (start < s.size() && std::isspace(static_cast<unsigned char>(s[start]))) {
        start++;
    }

    size_t end = s.size();
    while (end > start && std::isspace(static_cast<unsigned char>(s[end - 1]))) {
        end--;
    }

    return s.substr(start, end - start);
}

// parse only the vertex index from an OBJ face token
// examples:
// "3"      -> 2
// "3/7"    -> 2
// "3/7/9"  -> 2
// "3//9"   -> 2
bool parseFaceVertexToken(const std::string& token, int& vertexIndex) {
    if (token.empty()) return false;

    size_t slashPos = token.find('/');
    std::string vStr = (slashPos == std::string::npos) ? token : token.substr(0, slashPos);

    if (vStr.empty()) return false;

    try {
        int objIndex = std::stoi(vStr);

        // OBJ positive indices are 1-based
        // convert to 0-based
        if (objIndex > 0) {
            vertexIndex = objIndex - 1;
            return true;
        }

        // negative indices are legal in OBJ, but for simplicity you can reject them
        // or support them later if needed
        return false;
    } catch (...) {
        return false;
    }
}

}  // namespace

bool MeshLoader::loadOBJ(const std::string& filepath, Mesh& mesh) {
    mesh.clear();
 
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << filepath << "\n";
        return false;
    }

    std::string line;
    int lineNumber = 0;

    while (std::getline(file, line)) {
        lineNumber++;

        line = trim(line);

        // skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            Vertex v;
            if (!(iss >> v.x >> v.y >> v.z)) {
                std::cerr << "Invalid vertex at line " << lineNumber << "\n";
                return false;
            }
            mesh.addVertex(v);
        }
        else if (prefix == "f") {
            Face face;
            std::string token;

            while (iss >> token) {
                int vertexIndex;
                if (!parseFaceVertexToken(token, vertexIndex)) {
                    std::cerr << "Invalid face token '" << token
                              << "' at line " << lineNumber << "\n";
                    return false;
                }

                if (vertexIndex < 0 || vertexIndex >= static_cast<int>(mesh.vertices.size())) {
                    std::cerr << "Face vertex index out of range at line "
                              << lineNumber << "\n";
                    return false;
                }

                face.addVertexIndex(vertexIndex);
            }

            if (!face.isValid()) {
                std::cerr << "Face with fewer than 3 vertices at line "
                          << lineNumber << "\n";
                return false;
            }

            mesh.addFace(face);
        }
        else {
            // ignore all other prefixes: vt, vn, o, g, s, usemtl, mtllib, etc.
            continue;
        }
    }

    return true;
}
