#ifndef MESH_H
#define MESH_H

#include <vector>

using namespace std;

class Vector3 {
public:
    float x;
    float y;
    float z;

    Vector3();
    Vector3(float x, float y, float z);
};

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
    vector<int> vertexIndices;

    Face();
    explicit Face(const vector<int>& vertexIndices);

    void addVertexIndex(int vertexIndex);
    bool isValid() const;
};

class Mesh {
public:
    vector<Vertex> vertices;
    vector<Face> faces;
    Vector3 origin;
    float sideLength;

    Mesh();
    void clear();
    void addVertex(const Vertex& vertex);
    void addFace(const Face& face);

private:
    void updateOrigin();
};

#endif
