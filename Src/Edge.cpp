#include "Edge.hpp"

bool EdgeComparator::operator() (const Edge *a, const Edge *b) const {
    return a->Error () > b->Error ();
}

bool EdgeComparator::operator() (const Edge &a, const Edge &b) const {
    return a.Error () > b.Error ();
}

PairKey MakePairKey (Vertex const *a, Vertex const *b) {
    if (b->v < a->v) {
        std::swap (a, b);
    }
    return std::pair<Vec3, Vec3> (a->v, b->v);
}

Edge::Edge () : A (nullptr), B (nullptr), Removed (false), CachedError (-1) {}

Edge::Edge (Vertex const *a, Vertex const *b)
    : A (a), B (b), Removed (false), CachedError (-1) {
    if (b->v < a->v) {
        std::swap (A, B);
    }
}
bool Edge::operator== (const Edge &other) const {
    return A == other.A && B == other.B;
}

Matrix Edge::Quadric () const {
    return A->q + B->q;
}

std::unique_ptr<Vertex> Edge::ComputeNewVertex () const {
    return std::make_unique<Vertex> (ComputeNewVector (), Quadric ());
}

Vec3 Edge::ComputeNewVector () const {
    Matrix q = Quadric ();
    if (std::abs (q.Determinant ()) > EPSILON) {
        Vec3 v = q.QuadricVector ();
        if (!std::isnan (v.x) && !std::isnan (v.y) && !std::isnan (v.z)) {
            return v;
        }
    }
    // Cannot compute best vector with matrix
    // Look for best vector along edge
    Vec3 a = A->v;
    Vec3 b = B->v;
    Vec3 d = b - a;

    static constexpr size_t N = 32;
    double bestE = -1.0;
    Vec3 bestV;
    for (size_t i = 0; i <= N; ++i) {
        double t = static_cast<double> (i) / N;
        Vec3 v = a + (d * t);
        double e = q.QuadricError (v);
        if (bestE < 0 || e < bestE) {
            bestE = e;
            bestV = v;
        }
    }
    return bestV;
}

double Edge::Error () const {
    if (CachedError < 0) {
        CachedError = Quadric ().QuadricError (ComputeNewVector ());
    }
    return CachedError;
}