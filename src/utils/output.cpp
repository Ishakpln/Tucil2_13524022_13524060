#include "output.hpp"

#include <fstream>
#include <iostream>

using namespace std;

string buildOutputFilename(const string& path) {
    string filename = path;

    size_t lastSlash = filename.find_last_of("/\\");
    if (lastSlash != string::npos) {
        filename = filename.substr(lastSlash + 1);
    }

    size_t lastDot = filename.find_last_of('.');
    if (lastDot != string::npos) {
        filename = filename.substr(0, lastDot);
    }

    return filename + "_Voxelized.obj";
}

bool isValidMeshForOutput(const Mesh& mesh) {
    for (size_t i = 0; i < mesh.faces.size(); i++) {
        const Face& face = mesh.faces[i];

        if (!face.isValid()) {
            return false;
        }

        for (size_t j = 0; j < face.vertexIndices.size(); j++) {
            int vertexIndex = face.vertexIndices[j];
            int countVertex = mesh.vertices.size();

            if (vertexIndex < 0 || vertexIndex >= countVertex) {
                return false;   
            }
        }
    }

    return true;
}


bool MeshOutput::saveOBJ(const Mesh& mesh, const string& filename) {
    if (!isValidMeshForOutput(mesh)) {
        cerr << "NOT VALID\n";
        return false;
    }

    const string outputPath = "test/" + buildOutputFilename(filename);
    ofstream file(outputPath);
    if (!file.is_open()) {
        cerr << "Failed to open output OBJ file: " << outputPath << "\n";
        return false;
    }

    for (size_t i = 0; i < mesh.vertices.size(); i++) {
        const Vertex& vertex = mesh.vertices[i];
        file << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
    }

    for (size_t i = 0; i < mesh.faces.size(); i++) {
        const Face& face = mesh.faces[i];
        file << "f";

        for (size_t j = 0; j < face.vertexIndices.size(); j++) {
            file << " " << (face.vertexIndices[j] + 1);
        }

        file << "\n";
    }

    return true;
}
