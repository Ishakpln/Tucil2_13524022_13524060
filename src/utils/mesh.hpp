#ifndef MESH_H
#define MESH_H

#include <vector>

class Vertex {
public:
    float x;
    float y;
    float z;

    Vertex();
    Vertex(float x, float y, float z);
};

class Face {
public:
    std::vector<int> vertexIndices;

    Face();
    explicit Face(const std::vector<int>& vertexIndices);

    void addVertexIndex(int vertexIndex);
    bool isValid() const;
};

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<Face> faces;

    Mesh();
    void clear();
    void addVertex(const Vertex& vertex);
    void addFace(const Face& face);
};

#endif
