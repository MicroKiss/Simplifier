#pragma once

#include "Edge.hpp"
#include "Geometry.hpp"
#include <map>
#include <set>
#include <unordered_set>

std::map<Vec3, Vertex> CreateVertices (Mesh const &input);
Mesh Simplify (Mesh const &input, double factor);
Mesh ConstructMesh (std::map<Vertex const *, std::vector<Face const *>> const &vertexFaces);