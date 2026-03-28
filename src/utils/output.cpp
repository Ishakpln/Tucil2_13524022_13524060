#include "output.hpp"

#include <fstream>
#include <iostream>

using namespace std;

namespace {

string buildOutputFilename(const string& path) {
    string filename = path;

    size_t lastSlash = filename.find_last_of("\\/");
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
    const vector<Face>& faces = mesh.getFaces();
    const vector<Vertex>& vertices = mesh.getVertices();

    for (size_t i = 0; i < faces.size(); i++) {
        const Face& face = faces[i];
        const vector<int>& vertexIndices = face.getVertexIndices();

        if (!face.isValid()) {
            return false;
        }

        for (size_t j = 0; j < vertexIndices.size(); j++) {
            int vertexIndex = vertexIndices[j];
            int countVertex = static_cast<int>(vertices.size());

            if (vertexIndex < 0 || vertexIndex >= countVertex) {
                return false;
            }
        }
    }

    return true;
}

}

string MeshOutput::buildOutputPath(const string& filename) {
    return "test/" + buildOutputFilename(filename);
}

bool MeshOutput::saveOBJ(const Mesh& mesh, const string& filename) {
    if (!isValidMeshForOutput(mesh)) {
        cerr << "NOT VALID\n";
        return false;
    }

    const vector<Vertex>& vertices = mesh.getVertices();
    const vector<Face>& faces = mesh.getFaces();
    const string outputPath = buildOutputPath(filename);

    ofstream file(outputPath);
    if (!file.is_open()) {
        cerr << "Failed to open output OBJ file: " << outputPath << "\n";
        return false;
    }

    for (size_t i = 0; i < vertices.size(); i++) {
        const Vertex& vertex = vertices[i];
        file << "v " << vertex.positions.x << " " << vertex.positions.y << " " << vertex.positions.z << "\n";
    }

    for (size_t i = 0; i < faces.size(); i++) {
        const Face& face = faces[i];
        const vector<int>& vertexIndices = face.getVertexIndices();
        file << "f";

        for (size_t j = 0; j < vertexIndices.size(); j++) {
            file << " " << (vertexIndices[j] + 1);
        }

        file << "\n";
    }

    return true;
}
