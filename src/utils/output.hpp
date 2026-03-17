#ifndef MESH_OUTPUT_H
#define MESH_OUTPUT_H

#include "mesh.hpp"

#include <string>

using namespace std;

class MeshOutput {
public:
    static bool saveOBJ(const Mesh& mesh, const string& filename);
};

#endif
