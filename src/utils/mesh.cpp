#include "mesh.hpp"
#include "octree.hpp"

#include <algorithm>
#include <cassert>

Face::Face() {}

Face::Face(const vector<int>& indices) : vertexIndices(indices) {}

const vector<int>& Face::getVertexIndices() const {
    return vertexIndices;
}

void Face::addVertexIndex(int vertexIndex) {
    vertexIndices.push_back(vertexIndex);
}

bool Face::isValid() const {
    return vertexIndices.size() >= 3;
}

Mesh::Mesh() : origin(Vector3{}), boundingBox(AABB{}) {}

Mesh::Mesh(const vector<Vertex>& l_vertices, const vector<Face>& l_faces) : vertices(l_vertices), faces(l_faces) {
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

const vector<Vertex>& Mesh::getVertices() const {
    return vertices;
}

const vector<Face>& Mesh::getFaces() const {
    return faces;
}

const Vector3& Mesh::getOrigin() const {
    return origin;
}

const AABB& Mesh::getBoundingBox() const {
    return boundingBox;
}

void Mesh::updateOrigin() {
    if (vertices.empty()) {
        origin = Vector3{};
        return;
    }

    origin = boundingBox.center();
}

void Mesh::updateAABB() {
    boundingBox = computeAABB(vertices);
}

void Mesh::updateMesh() {
    updateAABB();
    updateOrigin();
}

AABB AABB::computeBoxAABB() const {
    float dx = maxBound.x - minBound.x;
    float dy = maxBound.y - minBound.y;
    float dz = maxBound.z - minBound.z;

    if (dx == dy && dy == dz) {
        return *this;
    }

    float side = max(dx, max(dy, dz));
    Vector3 c = center();
    Vector3 minCube{c.x - side / 2.0f, c.y - side / 2.0f, c.z - side / 2.0f};
    Vector3 maxCube{c.x + side / 2.0f, c.y + side / 2.0f, c.z + side / 2.0f};
    return AABB{minCube, maxCube};
}

Vector3 AABB::center() const {
    return Vector3{
        (maxBound.x + minBound.x) / 2.0f,
        (maxBound.y + minBound.y) / 2.0f,
        (maxBound.z + minBound.z) / 2.0f
    };
}

Vector3 AABB::halfSide() const {
    return Vector3{
        (maxBound.x - minBound.x) / 2.0f,
        (maxBound.y - minBound.y) / 2.0f,
        (maxBound.z - minBound.z) / 2.0f
    };
}

bool AABB::aabbCollisionDetection(const AABB& box) const {
    bool collideX = (maxBound.x < box.minBound.x) || (minBound.x > box.maxBound.x);
    if (collideX) {
        return false;
    }

    bool collideY = (maxBound.y < box.minBound.y) || (minBound.y > box.maxBound.y);
    if (collideY) {
        return false;
    }

    bool collideZ = (maxBound.z < box.minBound.z) || (minBound.z > box.maxBound.z);
    if (collideZ) {
        return false;
    }

    return true;
}

AABB computeAABB(const vector<Vertex>& vertices) {
    AABB boundingBox{};
    if (vertices.empty()) {
        return boundingBox;
    }

    boundingBox.minBound = vertices[0].positions;
    boundingBox.maxBound = vertices[0].positions;

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

constexpr float EPSILON = 1e-6f;

bool triangleBoxOverlapTest(AABB box, Vertex v0, Vertex v1, Vertex v2) {

    Vector3 centerBox = box.center();

    v0.positions -= centerBox;
    v1.positions -= centerBox;
    v2.positions -= centerBox;

    box.minBound -= centerBox;
    box.maxBound -= centerBox;

    AABB triangleBB{computeAABB({v0, v1, v2})};
    if (!box.aabbCollisionDetection(triangleBB)) {
        return false;
    }

    Vector3 f0{v1.positions - v0.positions};
    Vector3 f1{v2.positions - v1.positions};
    Vector3 f2{v0.positions - v2.positions};
    Vector3 n = cross(f0, f1);

    float r = box.maxBound.x * std::abs(n.x) + box.maxBound.y * std::abs(n.y) + box.maxBound.z * std::abs(n.z);
    float s = dot(n, v0.positions);
    if (!(std::abs(s) <= r + EPSILON)) {
        return false;
    }

    auto isSeparatedOnAxis = [&](const Vector3& axis) {
        float p0 = dot(axis, v0.positions);
        float p1 = dot(axis, v1.positions);
        float p2 = dot(axis, v2.positions);
        float pMin = std::min({p0, p1, p2});
        float pMax = std::max({p0, p1, p2});

        float axisRadius = box.maxBound.x * std::abs(axis.x)
                         + box.maxBound.y * std::abs(axis.y)
                         + box.maxBound.z * std::abs(axis.z);

        return (pMin > axisRadius + EPSILON) || (pMax < -axisRadius - EPSILON);
    };

    if (isSeparatedOnAxis(Vector3{0.0f, -f0.z, f0.y})) return false;
    if (isSeparatedOnAxis(Vector3{0.0f, -f1.z, f1.y})) return false;
    if (isSeparatedOnAxis(Vector3{0.0f, -f2.z, f2.y})) return false;

    if (isSeparatedOnAxis(Vector3{f0.z, 0.0f, -f0.x})) return false;
    if (isSeparatedOnAxis(Vector3{f1.z, 0.0f, -f1.x})) return false;
    if (isSeparatedOnAxis(Vector3{f2.z, 0.0f, -f2.x})) return false;

    if (isSeparatedOnAxis(Vector3{-f0.y, f0.x, 0.0f})) return false;
    if (isSeparatedOnAxis(Vector3{-f1.y, f1.x, 0.0f})) return false;
    if (isSeparatedOnAxis(Vector3{-f2.y, f2.x, 0.0f})) return false;

    return true;
}

static void recurseMeshing(Mesh& mesh, const OctreeNode& node) {
    if (node.isLeaf() && node.isFilled) {       
        int idx = static_cast<int>(mesh.getVertices().size());
        mesh.addVertex({node.cube.minBound});
        mesh.addVertex({Vector3{node.cube.maxBound.x, node.cube.minBound.y, node.cube.minBound.z}});
        mesh.addVertex({Vector3{node.cube.minBound.x, node.cube.minBound.y, node.cube.maxBound.z}});
        mesh.addVertex({Vector3{node.cube.maxBound.x, node.cube.minBound.y, node.cube.maxBound.z}});
        mesh.addVertex({Vector3{node.cube.minBound.x, node.cube.maxBound.y, node.cube.minBound.z}});
        mesh.addVertex({Vector3{node.cube.maxBound.x, node.cube.maxBound.y, node.cube.minBound.z}});
        mesh.addVertex({Vector3{node.cube.minBound.x, node.cube.maxBound.y, node.cube.maxBound.z}});
        mesh.addVertex({node.cube.maxBound});

        mesh.addFace(Face({idx + 0, idx + 1, idx + 3}));
        mesh.addFace(Face({idx + 0, idx + 3, idx + 2}));
        mesh.addFace(Face({idx + 1, idx + 0, idx + 4}));
        mesh.addFace(Face({idx + 1, idx + 4, idx + 5}));
        mesh.addFace(Face({idx + 3, idx + 1, idx + 5}));
        mesh.addFace(Face({idx + 3, idx + 5, idx + 7}));
        mesh.addFace(Face({idx + 2, idx + 3, idx + 7}));
        mesh.addFace(Face({idx + 2, idx + 7, idx + 6}));
        mesh.addFace(Face({idx + 0, idx + 2, idx + 6}));
        mesh.addFace(Face({idx + 0, idx + 6, idx + 4}));
        mesh.addFace(Face({idx + 5, idx + 4, idx + 6}));
        mesh.addFace(Face({idx + 5, idx + 6, idx + 7}));
    } else {
        for (auto child : node.children) {
            recurseMeshing(mesh, *child.second);
        }
    }
}

Mesh voxelMeshing(const Octree& voxelized) {
    Mesh vMesh;
    recurseMeshing(vMesh, voxelized.getRoot());
    return vMesh;
}

