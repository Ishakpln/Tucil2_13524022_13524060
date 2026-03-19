#include "octree.hpp"

#include <cassert>

namespace {

unsigned long long binPositivePow(unsigned long long base, int exponent) {
    unsigned long long result = 1;
    while (exponent > 0) {
        if ((exponent & 1) != 0) {
            result *= base;
        }
        base *= base;
        exponent >>= 1;
    }
    return result;
}

}

Octree::Octree() : root(nullptr), maxDepth(0) {}

Octree::Octree(int maxDepthValue, const AABB& bounds)
    : root(createNode(bounds.computeBoxAABB())), maxDepth(maxDepthValue) {
    countDepth.resize(maxDepth + 1, 0);
    if (root != nullptr) {
        countDepth[0] = 1;
        root->isFilled = true;
    }
}

Octree::~Octree() {
    delete root;
    root = nullptr;
}

OctreeNode::~OctreeNode() {
    for (auto& child : children) {
        delete child.second;
        child.second = nullptr;
    }
}

const OctreeNode& Octree::getRoot() const {
    return *root;
}

const int& Octree::getMaxDepth() const {
    return maxDepth;
}

std::vector<int> Octree::getCountDepth() const {
    return countDepth;
}

OctreeNode* Octree::createNode(const AABB& bounds) {
    OctreeNode* newNode = new OctreeNode;
    newNode->cube = bounds.computeBoxAABB();
    return newNode;
}

void Octree::addChild(OctreeNode* parent, OctreeNode& child, int idx) {
    assert(idx >= 0 && idx <= 7 && "Octant id is not in range from 0-7");
    assert(parent != nullptr && "Parent node cannot be null");
    assert(parent->children.size() < 8 && "Parent node has more than 8 children nodes");

    child.depth = parent->depth + 1;
    parent->children.push_back({idx, &child});
    if (child.depth >= 0 && child.depth < static_cast<int>(countDepth.size())) {
        countDepth[child.depth]++;
    }
}

std::vector<int> Octree::amountOfNodsePrunedDepthN() const {
    std::vector<int> prunedCountDepth;
    prunedCountDepth.reserve(maxDepth + 1);

    for (int i = 0; i <= maxDepth; i++) {
        unsigned long long fullCount = binPositivePow(8ull, i);
        int currentCount = (i < static_cast<int>(countDepth.size())) ? countDepth[i] : 0;
        prunedCountDepth.push_back(static_cast<int>(fullCount) - currentCount);
    }

    return prunedCountDepth;
}

void Octree::subdivide(OctreeNode* parent, const Mesh& mesh, const std::vector<Face>& subMesh, int depth) {
    if (parent == nullptr || depth >= maxDepth) {
        return;
    }

    std::vector<Face> currentFaces = filterTrianglesInBox(mesh, subMesh, parent->cube);
    if (currentFaces.empty()) {
        parent->isFilled = false;
        return;
    }

    parent->isFilled = true;

    for (int i = 0; i < 8; i++) {
        AABB childCube = computeChildCube(*parent, i);
        std::vector<Face> subFaces = filterTrianglesInBox(mesh, currentFaces, childCube);
        if (!subFaces.empty()) {
            OctreeNode* childNode = createNode(childCube);
            childNode->isFilled = true;
            addChild(parent, *childNode, i);
            subdivide(childNode, mesh, subFaces, depth + 1);
        }
    }
}

AABB computeChildCube(const OctreeNode& parent, int idx) {
    const Vector3& minBound = parent.cube.minBound;
    const Vector3& maxBound = parent.cube.maxBound;
    Vector3 center = parent.cube.center();

    Vector3 childMin = minBound;
    Vector3 childMax = center;

    if ((idx & 1) != 0) {
        childMin.x = center.x;
        childMax.x = maxBound.x;
    }
    if ((idx & 2) != 0) {
        childMin.z = center.z;
        childMax.z = maxBound.z;
    }
    if ((idx & 4) != 0) {
        childMin.y = center.y;
        childMax.y = maxBound.y;
    }

    return AABB{childMin, childMax};
}

std::vector<Face> filterTrianglesInBox(const Mesh& mesh, const std::vector<Face>& subMesh, const AABB& boundingCube) {
    std::vector<Face> filteredFaces;
    if (subMesh.empty()) {
        return filteredFaces;
    }

    const std::vector<Vertex>& vertices = mesh.getVertices();
    for (const Face& face : subMesh) {
        std::vector<Vertex> polygon;
        const std::vector<int>& indices = face.getVertexIndices();
        polygon.reserve(indices.size());

        for (int vertexIndex : indices) {
            polygon.push_back(vertices[vertexIndex]);
        }

        assert(polygon.size() == 3 && "Jgn dihilangin, untuk skrg triangle dulu inputnya biar gampang debug");
        if (triangleBoxOverlapTest(boundingCube, polygon)) {
            filteredFaces.push_back(face);
        }
    }

    return filteredFaces;
}
