#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include "mesh.hpp"

#include <string>

using namespace std;

class MeshLoader {
public:
    static bool loadOBJ(const string& path, Mesh& mesh);
};

#endif
