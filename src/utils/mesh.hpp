#ifndef MESH_H
#define MESH_H

#include <stack>
#include <vector>

#include "geometry.hpp"

using namespace std;

class Octree;
struct OctreeNode;

struct Vertex {
    Vector3 positions{};
};

class Face {
private:
    vector<int> vertexIndices;

public:
    Face();
    explicit Face(const vector<int>& vertexIndices);
    const vector<int>& getVertexIndices() const;
    void addVertexIndex(int vertexIndex);
    bool isValid() const;
};

struct AABB {
    Vector3 minBound{};
    Vector3 maxBound{};

    AABB computeBoxAABB() const;
    Vector3 center() const;
    Vector3 halfSide() const;
    bool aabbCollisionDetection(const AABB& box) const;
};

AABB computeAABB(const vector<Vertex>& vertices);

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
    const vector<Vertex>& getVertices() const;
    const vector<Face>& getFaces() const;
    const Vector3& getOrigin() const;
    const AABB& getBoundingBox() const;
};

bool triangleBoxOverlapTest(AABB box, vector<Vertex> triangle);
Mesh voxelMeshing(const Octree& voxelized);

#endif
