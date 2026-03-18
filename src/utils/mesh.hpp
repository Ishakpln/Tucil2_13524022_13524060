#ifndef MESH_H
#define MESH_H

#include <vector>

using namespace std;

struct Vector3{
    float x{0.0f}, y{0.0f}, z{0.0f};
};

struct Vertex {
    Vector3 positions{};
};

class Face {
private:
    vector<int> vertexIndices;

public:
    Face();
    explicit Face(const vector<int>& vertexIndices);

    void addVertexIndex(int vertexIndex);
    bool isValid() const;
};

struct AABB{
    Vector3 min{};
    Vector3 max{};
};

class Mesh {
private:
    vector<Vertex> vertices;
    vector<Face> faces;
    Vector3 origin;
    AABB boundingBox;

    void updateOrigin();
    void updateAABB();

public:
    Mesh();
    Mesh(const vector<Vertex>& l_vertices, const vector<Face>& l_faces);
    void clear();
    void addVertex(const Vertex& vertex);
    void addFace(const Face& face);
    void updateMesh();
    vector<Vertex> getVertices() const;
    vector<Face> getFaces() const;
    Vector3 getOrigin() const;
    AABB getBoundingBox() const;

};

#endif
