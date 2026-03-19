#pragma once

#include <vector>

struct AABB;

class Octree{
private:
    OctreeNode* root;
    int maxDepth;
    vector<int> countDepth;

public:
    Octree();
    Octree(int maxDepth, const AABB& bounds);
    ~Octree();
    const int& getMaxDepth() const;
    const OctreeNode& getRoot() const;
    const vector<int>& getCountDepth() const;
    OctreeNode* createNode(const AABB& bounds);
    void addChild(OctreeNode* parent, OctreeNode& child, int idx);
    void subdivide(OctreeNode* parent, const Mesh& mesh, vector<Face> subMesh,int depth);
    vector<int> amountOfNodsePrunedDepthN();
};

struct OctreeNode{
    ~OctreeNode();

    AABB cube{};
    vector<pair<int, OctreeNode*>> children;
    bool isFilled{false};
    int depth{0};

    bool isLeaf() const {return children.empty();}

};


AABB computeChildCube(const OctreeNode& parent, int idx);
vector<Face> filterTrianglesInBox(const Mesh& mesh, vector<Face> subMesh, const AABB& boundingCube);