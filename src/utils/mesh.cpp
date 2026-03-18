#include "mesh.hpp"
#include <algorithm>


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

Mesh::Mesh() : origin(Vector3{}), boundingBox(AABB{}) {}
Mesh::Mesh(const vector<Vertex>& l_vertices, const vector<Face>& l_faces) {
    for(int i = 0; i < l_vertices.size(); i++){
        addVertex(l_vertices[i]);
    }
    for(int i = 0; i < l_faces.size(); i++){
        addFace(l_faces[i]);
    }
    updateMesh();
}


void Mesh::clear() {
    vertices.clear();
    faces.clear();
    origin = Vector3{};
    boundingBox = AABB{};
}

void Mesh::addVertex(const Vertex& vertex) {
    vertices.push_back(vertex);
}

void Mesh::addFace(const Face& face) {
    faces.push_back(face);
}

vector<Vertex> Mesh::getVertices() const {return vertices;}
vector<Face> Mesh::getFaces() const{return faces;}
Vector3 Mesh::getOrigin() const{return origin;} 
AABB Mesh::getBoundingBox() const {return boundingBox;}

void Mesh::updateOrigin() {
    if (vertices.empty()) {
        origin = Vector3{};
        return;
    }


    float dx = boundingBox.max.x - boundingBox.min.x;
    float dy = boundingBox.max.y - boundingBox.min.y;
    float dz = boundingBox.max.z - boundingBox.min.z;

    float centerX = (boundingBox.max.x + boundingBox.min.x) / 2.0f;
    float centerY = (boundingBox.max.y + boundingBox.min.y) / 2.0f;
    float centerZ = (boundingBox.max.z + boundingBox.min.z) / 2.0f;
    float halfSideX = dx / 2.0f;
    float halfSideY = dy / 2.0f;
    float halfSideZ = dz / 2.0f;

    origin = Vector3{centerX, centerY, centerZ};
}

void Mesh::updateAABB() {
    boundingBox.min.x = vertices[0].positions.x;
    boundingBox.min.y = vertices[0].positions.y;
    boundingBox.min.z = vertices[0].positions.z;
    boundingBox.max.x = vertices[0].positions.x;
    boundingBox.max.y = vertices[0].positions.y;
    boundingBox.max.z = vertices[0].positions.z;

    for (size_t i = 1; i < vertices.size(); i++) {
        boundingBox.min.x = min(boundingBox.min.x, vertices[i].positions.x);
        boundingBox.min.y = min(boundingBox.min.y, vertices[i].positions.y);
        boundingBox.min.z = min(boundingBox.min.z, vertices[i].positions.z);
        boundingBox.max.x = max(boundingBox.max.x, vertices[i].positions.x);
        boundingBox.max.y = max(boundingBox.max.y, vertices[i].positions.y);
        boundingBox.max.z = max(boundingBox.max.z, vertices[i].positions.z);
    }
}

void Mesh::updateMesh() {
    updateOrigin();
    updateAABB();
}