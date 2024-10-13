#pragma once
#include <vector>

static constexpr double EPSILON = 1e-6;

struct Vec3 {
    double x, y, z;

    Vec3 ();
    Vec3 (double x, double y, double z);

    double Length () const;
    double Dot (const Vec3 &b) const;
    Vec3 Cross (const Vec3 &b) const;
    Vec3 Normalize () const;
    bool operator== (const Vec3 &other) const;
    bool operator!= (const Vec3 &other) const;
    bool operator< (const Vec3 &b) const;
    Vec3 operator+ (const Vec3 &b) const;
    Vec3 operator- (const Vec3 &b) const;
    Vec3 operator* (double b) const;
};

class Matrix {
  public:
    double row0[4];
    double row1[4];
    double row2[4];
    double row3[4];

    Matrix ();

    Matrix (double x00, double x01, double x02, double x03,
            double x10, double x11, double x12, double x13,
            double x20, double x21, double x22, double x23,
            double x30, double x31, double x32, double x33);
    double QuadricError (const Vec3 &v) const;
    Vec3 QuadricVector () const;
    Matrix operator+ (const Matrix &b) const;
    Vec3 MulPosition (const Vec3 &b) const;
    double Determinant () const;
    Matrix Inverse () const;
};

struct Triangle {
    Vec3 v1, v2, v3;

    Triangle () {};
    Triangle (const Vec3 &v1, const Vec3 &v2, const Vec3 &v3);

    Matrix Quadric () const;
    Vec3 Normal () const;
};

using Mesh = std::vector<Triangle>;

struct Vertex {
    Vec3 v;
    Matrix q; // Error quadric matrix

    Vertex () {};
    Vertex (Vertex const &other) : v (other.v), q (other.q) {}
    Vertex (Vec3 const &v) : v (v) {}
    Vertex (Vec3 const &v, Matrix q) : v (v), q (q) {}
};

class Face {
  public:
    Vertex const *v1;
    Vertex const *v2;
    Vertex const *v3;
    mutable bool Removed = false;

    bool operator< (const Face &other) const;

    Face (Vertex const *v1, Vertex const *v2, Vertex const *v3);

    bool Degenerate () const;
    Vec3 Normal () const;
};