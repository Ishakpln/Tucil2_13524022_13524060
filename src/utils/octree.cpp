#include "octree.hpp"
#include "Mesh.hpp"
#include <queue>
#include <cassert>

Octree::Octree() : root(nullptr), maxDepth(0) {}
Octree::Octree(int maxDepth, const AABB& bounds) 
: root(createNode(bounds.computeBoxAABB())), maxDepth(maxDepth) {
    countDepth.resize(maxDepth+1, 0);
}

Octree::~Octree(){
    if(root != nullptr){
        delete root;
    }
}

OctreeNode::~OctreeNode(){
    if(!children.empty()){
        for(auto child : children){
            if(child.second != nullptr){
                delete child.second;
            }
        }
    }
}

const OctreeNode& Octree::getRoot() const{return *root;}
const int& Octree::getMaxDepth() const {return maxDepth;}
const vector<int>& Octree::getCountDepth() const {return countDepth;}

OctreeNode* Octree::createNode(const AABB& bounds){
    OctreeNode* newNode = new OctreeNode;
    newNode->cube = bounds.computeBoxAABB();
    return newNode;
}
void Octree::addChild(OctreeNode* parent, OctreeNode& child, int idx){
    assert((idx <= 7 && idx >= 0) && "Octant id is not in range from 0-7");
    assert(parent->children.size() < 8 && "Parent node has more than 8 children nodes");
    child.depth = parent->depth + 1;
    pair<int, OctreeNode*> id_child{idx, &child};
    parent->children.push_back(id_child);
    countDepth[child.depth]++;
}

vector<int> Octree::amountOfNodsePrunedDepthN(){
    vector<int> prunedCountDepth(maxDepth);
    for(int i = 0; i < maxDepth; i++){
        prunedCountDepth.push_back(binPositivePow(8, i) - countDepth[i]);
    }
    return prunedCountDepth;
}

void Octree::subdivide(OctreeNode* parent, const Mesh& mesh, vector<Face> subMesh, int depth){
    if(depth > maxDepth) return;

    if(filterTrianglesInBox(mesh, mesh.getFaces(), parent->cube).empty()){
        parent->isFilled = false;
    }else{
        for(int i = 0; i < 8; i++){
            AABB childCube{computeChildCube(*parent, i)};
            vector<Face> subFaces{filterTrianglesInBox(mesh, subMesh, childCube)};
            if(!subFaces.empty()){
                OctreeNode* childNode = createNode(childCube);
                addChild(parent, *childNode, i);
                childNode->isFilled = true;
                subdivide(childNode, mesh, subFaces, depth + 1);
            }
        }
    }
}

AABB computeChildCube(const OctreeNode& parent, int idx){
    const Vector3& min{parent.cube.minBound};
    const Vector3& max{parent.cube.maxBound};
    Vector3 center{(parent.cube.maxBound.x + parent.cube.minBound.x)/2,
                   (parent.cube.maxBound.y + parent.cube.minBound.y)/2,
                   (parent.cube.maxBound.z + parent.cube.minBound.z)/2};

    switch(idx){
        case 0:
            Vector3 minBB{min};
            Vector3 maxBB{center};
            return AABB {minBB, maxBB};
            break;
        case 1:
            Vector3 minBB{center.x, min.y, min.z};
            Vector3 maxBB{max.x, center.y, center.z};
            return AABB {minBB, maxBB};
            break;
        case 2:
            Vector3 minBB{min.x, min.y, center.z};
            Vector3 maxBB{center.x, center.y, max.z};
            return AABB {minBB, maxBB};
            break;
        case 3:
            Vector3 minBB{center.x, min.y, center.z};
            Vector3 maxBB{max.x, center.y, max.z};
            return AABB {minBB, maxBB};
            break;
        case 4:
            Vector3 minBB{min.x, center.y, min.z};
            Vector3 maxBB{center.x, max.y, center.z};
            return AABB {minBB, maxBB};
            break;
        case 5:
            Vector3 minBB{center.x, center.y, min.z};
            Vector3 maxBB{max.z, max.y, center.z};
            return AABB {minBB, maxBB};
            break;
        case 6:
            Vector3 minBB{min.x, center.y, center.z};
            Vector3 maxBB{center.x, max.y, max.z};
            return AABB {minBB, maxBB};
            break;
        case 7:
            Vector3 minBB{center.x, center.y, center.z};
            Vector3 maxBB{max.x, max.y, max.z};
            return AABB {minBB, maxBB};
            break;
    }
}

vector<Face> filterTrianglesInBox(const Mesh& mesh, vector<Face> subMesh, const AABB& boundingCube){
    if(subMesh.empty()) return;
    const vector<Vertex>& l_vertices{mesh.getVertices()};
    vector<Face> l_sub_faces;
    for (Face face : subMesh){
        vector<Vertex> polygon;
        for (int v_idx : face.getVertexIndices()){
            polygon.push_back(l_vertices[v_idx]);
        }
        triangleBoxOverlapTest(boundingCube, polygon);
    }

    return l_sub_faces;
}

static long long unsigned int binPositivePow(int b, int n){
    int res{1};
    for(int i = n; i > 0; i/=2){
        if(i % 2){
            res *= b;
        }
        b *= b;
    }

    return res;
}




