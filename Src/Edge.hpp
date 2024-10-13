#pragma once

#include "Geometry.hpp"
#include <cmath>
#include <memory>
#include <utility>

using PairKey = std::pair<Vec3, Vec3>;

PairKey MakePairKey (Vertex const *a, Vertex const *b);

class Edge {
  public:
    Vertex const *A;
    Vertex const *B;
    mutable bool Removed;
    mutable double CachedError;

    Edge ();
    Edge (Vertex const *a, Vertex const *b);
    bool operator== (const Edge &other) const;
    Matrix Quadric () const;
    std::unique_ptr<Vertex> ComputeNewVertex () const;
    Vec3 ComputeNewVector () const;
    double Error () const;
};

// for unordered_set
template <>
struct std::hash<Edge> {
    std::size_t operator() (const Edge &s) const noexcept {
        std::size_t h1 = std::hash<Vertex const *>{}(s.A);
        std::size_t h2 = std::hash<Vertex const *>{}(s.B);
        return h1 ^ (h2 << 1);
    }
};

struct EdgeComparator {
    bool operator() (Edge const &a, Edge const &b) const;
    bool operator() (const Edge *a, const Edge *b) const;
};