#include "octree.hpp"
#include "Mesh.hpp"
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

const vector<Vertex>& Mesh::getVertices() const {return vertices;}
const vector<Face>& Mesh::getFaces() const{return faces;}
const Vector3& Mesh::getOrigin() const{return origin;} 
const AABB& Mesh::getBoundingBox() const {return boundingBox;}

void Mesh::updateOrigin() {
    if (vertices.empty()) {
        origin = Vector3{};
        return;
    }

    origin = Vector3{boundingBox.center()};
}

void Mesh::updateAABB() {
    boundingBox = computeAABB(vertices);
}

void Mesh::updateMesh() {
    updateOrigin();
    updateAABB();
}

AABB AABB::computeBoxAABB() const{
    float dx = maxBound.x - minBound.x;
    float dy = maxBound.y - minBound.y;
    float dz = maxBound.z - minBound.z;
    if(dx == dy && dy == dz && dx == dz) return *this;
    float side = max(dx, max(dy, dz));
    Vector3 c{center()};
    Vector3 minCube{c.x - side/2 - 0.1f,
                c.y - side/2 - 0.1f,
                c.z - side/2 - 0.1f};
    Vector3 maxCube{c.x + side/2 + 0.1f,
                c.y + side/2 + 0.1f,
                c.z + side/2 + 0.1f};
    AABB boundingCube{minCube, maxCube};
}

Vector3 AABB::center() const{
    return Vector3{(maxBound.x + minBound.x)/2,
                   (maxBound.y + minBound.y)/2,
                   (maxBound.z + minBound.z)/2};
}

Vector3 AABB::halfSide() const{
    return Vector3{(maxBound.x - minBound.x)/2,
                   (maxBound.y - minBound.y)/2,
                   (maxBound.z - minBound.z)/2};
}

AABB computeAABB(const vector<Vertex>& vertices){
    AABB boundingBox{};
    boundingBox.minBound.x = vertices[0].positions.x;
    boundingBox.minBound.y = vertices[0].positions.y;
    boundingBox.minBound.z = vertices[0].positions.z;
    boundingBox.maxBound.x = vertices[0].positions.x;
    boundingBox.maxBound.y = vertices[0].positions.y;
    boundingBox.maxBound.z = vertices[0].positions.z;

    for (size_t i = 1; i < vertices.size(); i++) {
        boundingBox.minBound.x = min(boundingBox.minBound.x, vertices[i].positions.x);
        boundingBox.minBound.y = min(boundingBox.minBound.y, vertices[i].positions.y);
        boundingBox.minBound.z = min(boundingBox.minBound.z, vertices[i].positions.z);
        boundingBox.maxBound.x = max(boundingBox.maxBound.x, vertices[i].positions.x);
        boundingBox.maxBound.y = max(boundingBox.maxBound.y, vertices[i].positions.y);
        boundingBox.maxBound.z = max(boundingBox.maxBound.z, vertices[i].positions.z);
    }

    return boundingBox;
}

bool triangleBoxOverlapTest(AABB box, vector<Vertex> triangle){
    assert(triangle.size() == 3 && "Vector of vertices has length not 3(not triangle)");

    Vector3 centerBox = box.center();
    for(Vertex v : triangle){
        v.positions -= centerBox; 
    }
    box.minBound -= centerBox;
    box.maxBound -= centerBox;


    AABB triangleBB{computeAABB(triangle)};
    if(!box.aabbCollisionDetection(triangleBB)) return false;

    Vector3 f0 {triangle[1].positions - triangle[0].positions};
    Vector3 f1 {triangle[2].positions - triangle[1].positions};
    Vector3 f2 {triangle[0].positions - triangle[2].positions};
    Vector3 n = cross(f0, f1); 

    float r = box.maxBound.x*abs(n.x) + box.maxBound.y*abs(n.y) + box.maxBound.z*abs(n.z);

    float s = dot(n, triangle[0].positions);
    if(!abs(s) <= r) return false;

    Vector3 a{0, -f0.z, f0.y};
    float p0 {dot(a, triangle[0].positions)};
    float p2 {dot(a, triangle[2].positions)};
    r = box.maxBound.y*abs(a.y) + box.maxBound.z*abs(a.z);
    if(min(p0, p2) > r || max(p0, p2) < -r) return false;

    a = {0, -f1.z, f1.y};
    p0 = dot(a, triangle[0].positions);
    p2 = dot(a, triangle[2].positions);
    r = box.maxBound.y*abs(a.y) + box.maxBound.z*abs(a.z);
    if(min(p0, p2) > r || max(p0, p2) < -r) return false;

    a = {0, -f2.z, f2.y};
    p0 = dot(a, triangle[0].positions);
    p2 = dot(a, triangle[2].positions);
    r = box.maxBound.y*abs(a.y) + box.maxBound.z*abs(a.z);
    if(min(p0, p2) > r || max(p0, p2) < -r) return false;

    a = {f0.z, 0, -f0.x};
    p0 = dot(a, triangle[0].positions);
    p2 = dot(a, triangle[2].positions);
    r = box.maxBound.y*abs(a.y) + box.maxBound.z*abs(a.z);
    if(min(p0, p2) > r || max(p0, p2) < -r) return false;

    a = {f1.z, 0, -f1.x};
    p0 = dot(a, triangle[0].positions);
    p2 = dot(a, triangle[2].positions);
    r = box.maxBound.y*abs(a.y) + box.maxBound.z*abs(a.z);
    if(min(p0, p2) > r || max(p0, p2) < -r) return false;

    a = {f2.z, 0, -f2.x};
    p0 = dot(a, triangle[0].positions);
    p2 = dot(a, triangle[2].positions);
    r = box.maxBound.y*abs(a.y) + box.maxBound.z*abs(a.z);
    if(min(p0, p2) > r || max(p0, p2) < -r) return false;

    a = {-f0.y, f0.x, 0};
    p0 = dot(a, triangle[0].positions);
    p2 = dot(a, triangle[2].positions);
    r = box.maxBound.y*abs(a.y) + box.maxBound.z*abs(a.z);
    if(min(p0, p2) > r || max(p0, p2) < -r) return false;

    a = {-f0.y, f0.x, 0};
    p0 = dot(a, triangle[0].positions);
    p2 = dot(a, triangle[2].positions);
    r = box.maxBound.y*abs(a.y) + box.maxBound.z*abs(a.z);
    if(min(p0, p2) > r || max(p0, p2) < -r) return false;

    a = {-f0.y, f0.x, 0};
    p0 = dot(a, triangle[0].positions);
    p2 = dot(a, triangle[2].positions);
    r = box.maxBound.y*abs(a.y) + box.maxBound.z*abs(a.z);
    if(min(p0, p2) > r || max(p0, p2) < -r) return false;

    return true;

}

bool AABB::aabbCollisionDetection(AABB& box){
    bool collideX { (maxBound.x < box.minBound.x) || (minBound.x > box.maxBound.x) };
    if(collideX) return false;
    bool collideY { (maxBound.y < box.minBound.y) || (minBound.y > box.maxBound.y) };
    if(collideY) return false;
    bool collideZ { (maxBound.z < box.minBound.z) || (minBound.z > box.maxBound.z) };
    if(collideZ) return false;

    return true;
}

Mesh voxelMeshing(const Octree& voxelized) {
    Mesh vMesh;
    recurseMeshing(vMesh, voxelized.getRoot());
    return vMesh;
}

static void recurseMeshing(Mesh& mesh, const OctreeNode& node){
    if(node.isLeaf() && node.isFilled) {
        int idx = mesh.getVertices().size();
        mesh.addVertex({node.cube.minBound});
        mesh.addVertex({node.cube.maxBound.x, node.cube.minBound.y, node.cube.minBound.z});
        mesh.addVertex({node.cube.minBound.x, node.cube.minBound.y, node.cube.maxBound.z});
        mesh.addVertex({node.cube.maxBound.x, node.cube.minBound.y, node.cube.maxBound.z});
        mesh.addVertex({node.cube.minBound.x, node.cube.maxBound.y, node.cube.minBound.z});
        mesh.addVertex({node.cube.maxBound.x, node.cube.maxBound.y, node.cube.minBound.z});
        mesh.addVertex({node.cube.minBound.x, node.cube.maxBound.y, node.cube.maxBound.z});
        mesh.addVertex({node.cube.maxBound});

        mesh.addFace(Face({idx+0,idx+1,idx+2,idx+3}));
        mesh.addFace(Face({idx+0,idx+1,idx+5,idx+4}));
        mesh.addFace(Face({idx+1,idx+2,idx+7,idx+5}));
        mesh.addFace(Face({idx+2,idx+3,idx+7,idx+6}));
        mesh.addFace(Face({idx+0,idx+2,idx+6,idx+5}));
        mesh.addFace(Face({idx+4,idx+5,idx+7,idx+6}));
    }else{
        for(auto child : node.children){
            recurseMeshing(mesh, *child.second);
        }
    }
}