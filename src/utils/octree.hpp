#pragma once

#include <vector>
#include <thread>

struct OctreeNode {
    ~OctreeNode();

    AABB cube{};
    std::vector<std::pair<int, OctreeNode*>> children;
    bool isFilled{false};
    int depth{0};

    bool isLeaf() const { return children.empty(); }
};

class Octree {
private:
    OctreeNode* root;
    int maxDepth;
    std::vector<int> countDepth;

    void subdivide_recurse(OctreeNode& parent, const Mesh& mesh, const std::vector<Face>& subMesh, int depth);

public:
    Octree();
    Octree(int maxDepth, const AABB& bounds);
    ~Octree();

    const int& getMaxDepth() const;
    const OctreeNode& getRoot() const;
    std::vector<int> getCountDepth() const;

    OctreeNode* createNode(const AABB& bounds);
    void addChild(OctreeNode* parent, OctreeNode& child, int idx);
    void subdivide(const Mesh& mesh);
    std::vector<int> amountOfNodsePrunedDepthN() const;
};

AABB computeChildCube(const OctreeNode& parent, int idx);
std::vector<Face> filterTrianglesInBox(const Mesh& mesh, const std::vector<Face>& subMesh, const AABB& boundingCube);
