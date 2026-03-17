#include "mesh.hpp"

#include <algorithm>

using namespace std;

Vector3::Vector3() : x(0.0f), y(0.0f), z(0.0f) {}

Vector3::Vector3(float xVal, float yVal, float zVal)
    : x(xVal), y(yVal), z(zVal) {}

Vertex::Vertex() : x(0.0f), y(0.0f), z(0.0f) {}

Vertex::Vertex(float xVal, float yVal, float zVal)
    : x(xVal), y(yVal), z(zVal) {}

Face::Face() {};
Face::Face(const vector<int>& indices) : vertexIndices(indices) {}

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

Mesh::Mesh() : origin(), sideLength(0.0f) {}

void Mesh::clear() {
    vertices.clear();
    faces.clear();
    origin = Vector3();
    sideLength = 0.0f;
}

void Mesh::addVertex(const Vertex& vertex) {
    vertices.push_back(vertex);
    updateOrigin();
}

void Mesh::addFace(const Face& face) {
    faces.push_back(face);
}

void Mesh::updateOrigin() {
    if (vertices.empty()) {
        origin = Vector3();
        return;
    }

    float minX = vertices[0].x;
    float minY = vertices[0].y;
    float minZ = vertices[0].z;
    float maxX = vertices[0].x;
    float maxY = vertices[0].y;
    float maxZ = vertices[0].z;

    for (size_t i = 1; i < vertices.size(); i++) {
        minX = min(minX, vertices[i].x);
        minY = min(minY, vertices[i].y);
        minZ = min(minZ, vertices[i].z);
        maxX = max(maxX, vertices[i].x);
        maxY = max(maxY, vertices[i].y);
        maxZ = max(maxZ, vertices[i].z);
    }

    float dx = maxX - minX;
    float dy = maxY - minY;
    float dz = maxZ - minZ;
    sideLength = max(dx, max(dy, dz));

    float cX = (minX + maxX) / 2.0f;
    float centerY = (minY + maxY) / 2.0f;
    float centerZ = (minZ + maxZ) / 2.0f;
    float hSide = sideLength / 2.0f;

    origin = Vector3(cX - hSide, centerY - hSide, centerZ - hSide);
}
