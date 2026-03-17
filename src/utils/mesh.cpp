#include "mesh.hpp"

Vertex::Vertex() : x(0.0f), y(0.0f), z(0.0f) {}

Vertex::Vertex(float xVal, float yVal, float zVal)
    : x(xVal), y(yVal), z(zVal) {}

Face::Face() {};
Face::Face(const std::vector<int>& indices) : vertexIndices(indices) {}

void Face::addVertexIndex(int vertexIndex) {
    vertexIndices.push_back(vertexIndex);
}

bool Face::isValid() const {
    if (vertexIndices.size() >= 3) {
        return true;
    }
    else {
        return false;
    }
}

Mesh::Mesh() {};

void Mesh::clear() {
    vertices.clear();
    faces.clear();
}

void Mesh::addVertex(const Vertex& vertex) {
    vertices.push_back(vertex);
}

void Mesh::addFace(const Face& face) {
    faces.push_back(face);
}
