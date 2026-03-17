#ifndef MESH_LOADER_H
#define MESH_LOADER_H

#include "mesh.hpp"

#include <string>

class MeshLoader {
public:
    static bool loadOBJ(const std::string& filepath, Mesh& mesh);
};

#endif
