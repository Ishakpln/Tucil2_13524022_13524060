#include "mesh.hpp"
#include "octree.hpp"

constexpr size_t minFacesConcurrencyCutoff = 128;

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

Octree::Octree() : root(nullptr), maxDepth(0) {}

Octree::Octree(int maxDepthValue, const AABB& bounds)
    : root(createNode(bounds)), maxDepth(maxDepthValue) {
    root->cube = bounds.computeBoxAABB();
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
    newNode->cube = bounds;
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

void Octree::subdivide(const Mesh& mesh) {
    if (mesh.getFaces().empty()) {
        root->isFilled = false;
        return;
    }

    root->isFilled = true;

    subdivide_recurse(*root, mesh, mesh.getFaces(), 0);
}

void Octree::subdivide_recurse(OctreeNode& parent, const Mesh& mesh, const std::vector<Face>& subMesh, int depth){
    if(depth >= maxDepth){
        return;
    }
    
    if(subMesh.empty()){
        parent.isFilled = false;
        return;
    }

    std::vector<AABB> childCubes;
    for(int i = 0; i < 8; i++){
        childCubes.push_back(computeChildCube(parent, i));
    }

    std::vector<std::vector<Face>> childSubMeshes(8);

    if(subMesh.size() > minFacesConcurrencyCutoff){
        std::vector<jthread> workers;
        {
            for(int i = 0; i < 8; i++){
                workers.emplace_back([i, &childSubMeshes, &mesh, &subMesh, &childCubes]()
                    {childSubMeshes[i] = filterTrianglesInBox(mesh, subMesh, childCubes[i]);}
                );
            }
        }
    }else{
        for(int i = 0; i < 8; i++){
            childSubMeshes[i] = filterTrianglesInBox(mesh, subMesh, childCubes[i]);
        }
    }

        for (int i = 0; i < 8; i++) {
            if(!childSubMeshes[i].empty()){
                OctreeNode* childNode = createNode(childCubes[i]);
                childNode->isFilled = true;
                addChild(&parent, *childNode, i);
                subdivide_recurse(*childNode, mesh, childSubMeshes[i], depth + 1);
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
    std::vector<Face> filteredFaces{};
    if (subMesh.empty()) {
        return filteredFaces;
    }

    const std::vector<Vertex>& vertices = mesh.getVertices();
    for (const Face& face : subMesh) {
        const std::vector<int>& indices = face.getVertexIndices();

        if (triangleBoxOverlapTest(boundingCube, vertices[indices[0]], vertices[indices[1]], vertices[indices[2]])) {
            filteredFaces.push_back(face);
        }
    }

    return filteredFaces;
}
