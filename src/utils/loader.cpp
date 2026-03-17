#include "loader.hpp"

#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>

using namespace std;

namespace {

string trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace(static_cast<unsigned char>(s[start]))) {
        start++;
    }

    size_t end = s.size();
    while (end > start && isspace(static_cast<unsigned char>(s[end - 1]))) {
        end--;
    }

    return s.substr(start, end - start);
}

bool parseFaceVertexToken(const string& token, int& vertexIndex) {
    if (token.empty()) return false;

    size_t slashPos = token.find('/');
    string vStr = (slashPos == string::npos) ? token : token.substr(0, slashPos);

    if (vStr.empty()) return false;

    try {
        int objIndex = stoi(vStr);

        if (objIndex > 0) {
            vertexIndex = objIndex - 1;
            return true;
        }

        return false;
    } catch (...) {
        return false;
    }
}

}

bool MeshLoader::loadOBJ(const string& path, Mesh& mesh) {
    mesh.clear();
 
    ifstream file(path);
    if (!file.is_open()) {
        cerr << "Failed to open file!\n";
        return false;
    }

    string line;
    int lineNumber = 0;

    while (getline(file, line)) {
        lineNumber++;

        line = trim(line);

        if (line.empty() || line[0] == '#') {
            continue;
        }

        istringstream iss(line);
        string prefix = "";

        for (int i = 0; i < line.length(); i++) {
            if (line[i] == ' ') {
                break;
            }
            prefix += line[i];
        }

        iss >> prefix;

        if (prefix == "v") {
            Vertex v;
            if (!(iss >> v.x >> v.y >> v.z)) {
                cerr << "not valid\n";
                return false;
            }
            mesh.addVertex(v);
        }
        else if (prefix == "f") {
            Face face;
            string token;

            while (iss >> token) {
                int vertexIndex;
                if (!parseFaceVertexToken(token, vertexIndex)) {
                    cerr << "not valid\n";
                    return false;
                }

                if (vertexIndex < 0 || vertexIndex >= static_cast<int>(mesh.vertices.size())) {
                    cerr << "not valid\n";
                    return false;
                }

                face.addVertexIndex(vertexIndex);
            }

            if (!face.isValid()) {
                cerr << "not valid\n";
                return false;
            }

            mesh.addFace(face);
        }
        else {
            continue;
        }
    }

    return true;
}
