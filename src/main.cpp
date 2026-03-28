#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

#include "utils/loader.hpp"
#include "utils/octree.hpp"
#include "utils/output.hpp"

using namespace std;

namespace {

void printDivider() {
    cout << "============================================================\n";
}

void printHeader() {
    printDivider();
    cout << "                     Voxelizator 3D\n";
    printDivider();
}

double estimateMeshSizeMb(const Mesh& mesh) {
    size_t bytes = mesh.getVertices().size() * sizeof(Vertex) + mesh.getFaces().size() * sizeof(Face);
    return static_cast<double>(bytes) / (1024.0 * 1024.0);
}

int countIntersectedNodes(const OctreeNode* node) {
    if (node == nullptr) {
        return 0;
    }

    int count = node->isFilled ? 1 : 0;
    for (const auto& child : node->children) {
        count += countIntersectedNodes(child.second);
    }
    return count;
}

void printEmptyState() {
    printHeader();
    cout << "Please upload your 3D model\n\n";
}

void printLoadedState(const Mesh& mesh, const string& filename) {
    printHeader();
    cout << "Loaded file : " << filename << "\n";
    cout << "Vertices    : " << mesh.getVertices().size() << "\n";
    cout << "Faces       : " << mesh.getFaces().size() << "\n";
    cout << "Size        : " << fixed << setprecision(3) << estimateMeshSizeMb(mesh) << " mb\n\n";
}

void printResultState(const Mesh& mesh,
                      int depth,
                      int octreeCount,
                      double runtimeSeconds,
                      const string& outputInfo) {
    printHeader();
    cout << "Vertices    : " << mesh.getVertices().size() << "\n";
    cout << "Faces       : " << mesh.getFaces().size() << "\n";
    cout << "Size        : " << fixed << setprecision(3) << estimateMeshSizeMb(mesh) << " mb\n";
    cout << "Octree      : " << octreeCount << "\n";
    cout << "Max Depth   : " << depth << "\n";
    cout << "Runtime     : " << fixed << setprecision(3) << runtimeSeconds << " sec\n";
    cout << "Output      : " << outputInfo << "\n\n";
}

bool yesORno(const string& prompt) {
    while (true) {
        cout << prompt << " (y/n): ";
        string input;
        getline(cin, input);

        if (input == "y" || input == "Y") {
            return true;
        }
        if (input == "n" || input == "N") {
            return false;
        }

        cout << "Please enter y or n.\n";
    }
}

int askDepth() {
    while (true) {
        cout << "Insert maximum depth value: ";
        int depth = 0;
        if (cin >> depth && depth >= 0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return depth;
        }
        
        cout << "Depth cannot be a negative number.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

}

int main() {
    while (true) {
        printEmptyState();

        cout << "Input OBJ file path: ";
        string path;
        getline(cin, path);

        if (path.empty()) {
            cout << "No file path found.\n";
            if (!yesORno("Try again")) {
                break;
            }
            continue;
        }

        Mesh mesh;
        if (!MeshLoader::loadOBJ(path, mesh)) {
            cout << "Failed to load OBJ file.\n";
            if (!yesORno("Try another file")) {
                break;
            }
            continue;
        }

        mesh.updateMesh();
        printLoadedState(mesh, path);

        int maxDepth = askDepth();

        auto startTime = chrono::high_resolution_clock::now();
        Octree octree(maxDepth, mesh.getBoundingBox());
        octree.subdivide(mesh);
        Mesh voxelMesh = voxelMeshing(octree);
        auto endTime = chrono::high_resolution_clock::now();

        chrono::duration<double> duration = endTime - startTime;
        int octreeCount = countIntersectedNodes(&octree.getRoot());
        string outputInfo = "not saved";

        if (yesORno("Do you want to save file")) {
            if (MeshOutput::saveOBJ(voxelMesh, path)) {
                outputInfo = "saved to " + MeshOutput::buildOutputPath(path);
            } else {
                outputInfo = "failed to save output obj file";
            }
        }

        printResultState(voxelMesh, maxDepth, octreeCount, duration.count(), outputInfo);

        if (!yesORno("Processing another file")) {
            break;
        }
    }

    return 0;
}
