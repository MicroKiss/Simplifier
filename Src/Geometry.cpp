#include "Geometry.hpp"
#include <cmath>

Vec3::Vec3 () : x (0), y (0), z (0) {}
Vec3::Vec3 (double x, double y, double z) : x (x), y (y), z (z) {}

double Vec3::Length () const {
    return std::sqrt (x * x + y * y + z * z);
}

double Vec3::Dot (const Vec3 &b) const {
    return x * b.x + y * b.y + z * b.z;
}

Vec3 Vec3::Cross (const Vec3 &b) const {
    return {y * b.z - z * b.y,
            z * b.x - x * b.z,
            x * b.y - y * b.x};
}

Vec3 Vec3::Normalize () const {
    double d = Length ();
    return {x / d, y / d, z / d};
}

bool Vec3::operator== (const Vec3 &other) const {
    return std::abs (x - other.x) < EPSILON &&
           std::abs (y - other.y) < EPSILON &&
           std::abs (z - other.z) < EPSILON;
}

Vec3 Vec3::operator+ (const Vec3 &b) const {
    return {x + b.x, y + b.y, z + b.z};
}

Vec3 Vec3::operator- (const Vec3 &b) const {
    return {x - b.x, y - b.y, z - b.z};
}

Vec3 Vec3::operator* (double b) const {
    return {x * b, y * b, z * b};
}

bool Vec3::operator< (const Vec3 &b) const {
    if (x != b.x)
        return x < b.x;

    if (y != b.y)
        return y < b.y;

    return z < b.z;
}

Matrix::Matrix () : row0{0, 0, 0, 0},
                    row1{0, 0, 0, 0},
                    row2{0, 0, 0, 0},
                    row3{0, 0, 0, 0} {}

Matrix::Matrix (double x00, double x01, double x02, double x03,
                double x10, double x11, double x12, double x13,
                double x20, double x21, double x22, double x23,
                double x30, double x31, double x32, double x33)
    : row0{x00, x01, x02, x03},
      row1{x10, x11, x12, x13},
      row2{x20, x21, x22, x23},
      row3{x30, x31, x32, x33} {}

double Matrix::QuadricError (Vec3 const &v) const {
    return (v.x * row0[0] * v.x + v.y * row1[0] * v.x + v.z * row2[0] * v.x + row3[0] * v.x +
            v.x * row0[1] * v.y + v.y * row1[1] * v.y + v.z * row2[1] * v.y + row3[1] * v.y +
            v.x * row0[2] * v.z + v.y * row1[2] * v.z + v.z * row2[2] * v.z + row3[2] * v.z +
            v.x * row0[3] + v.y * row1[3] + v.z * row2[3] + row3[3]);
}

Vec3 Matrix::QuadricVector () const {
    Matrix b = {
        row0[0], row0[1], row0[2], row0[3],
        row1[0], row1[1], row1[2], row1[3],
        row2[0], row2[1], row2[2], row2[3],
        0, 0, 0, 1};
    return b.Inverse ().MulPosition (Vec3 ());
}

Matrix Matrix::operator+ (const Matrix &b) const {
    return Matrix{
        row0[0] + b.row0[0], row0[1] + b.row0[1], row0[2] + b.row0[2], row0[3] + b.row0[3],
        row1[0] + b.row1[0], row1[1] + b.row1[1], row1[2] + b.row1[2], row1[3] + b.row1[3],
        row2[0] + b.row2[0], row2[1] + b.row2[1], row2[2] + b.row2[2], row2[3] + b.row2[3],
        row3[0] + b.row3[0], row3[1] + b.row3[1], row3[2] + b.row3[2], row3[3] + b.row3[3]};
}

Vec3 Matrix::MulPosition (const Vec3 &b) const {
    double x = row0[0] * b.x + row0[1] * b.y + row0[2] * b.z + row0[3];
    double y = row1[0] * b.x + row1[1] * b.y + row1[2] * b.z + row1[3];
    double z = row2[0] * b.x + row2[1] * b.y + row2[2] * b.z + row2[3];
    return Vec3{x, y, z};
}

double Matrix::Determinant () const {
    return (row0[0] * row1[1] * row2[2] * row3[3] - row0[0] * row1[1] * row2[3] * row3[2] +
            row0[0] * row1[2] * row2[3] * row3[1] - row0[0] * row1[2] * row2[1] * row3[3] +
            row0[0] * row1[3] * row2[1] * row3[2] - row0[0] * row1[3] * row2[2] * row3[1] -
            row0[1] * row1[2] * row2[3] * row3[0] + row0[1] * row1[2] * row2[0] * row3[3] -
            row0[1] * row1[3] * row2[0] * row3[2] + row0[1] * row1[3] * row2[2] * row3[0] -
            row0[1] * row1[0] * row2[2] * row3[3] + row0[1] * row1[0] * row2[3] * row3[2] +
            row0[2] * row1[3] * row2[0] * row3[1] - row0[2] * row1[3] * row2[1] * row3[0] +
            row0[2] * row1[0] * row2[1] * row3[3] - row0[2] * row1[0] * row2[3] * row3[1] +
            row0[2] * row1[1] * row2[3] * row3[0] - row0[2] * row1[1] * row2[0] * row3[3] -
            row0[3] * row1[0] * row2[1] * row3[2] + row0[3] * row1[0] * row2[2] * row3[1] -
            row0[3] * row1[1] * row2[2] * row3[0] + row0[3] * row1[1] * row2[0] * row3[2] -
            row0[3] * row1[2] * row2[0] * row3[1] + row0[3] * row1[2] * row2[1] * row3[0]);
}

Matrix Matrix::Inverse () const {
    Matrix m;
    double r = 1. / Determinant ();
    m.row0[0] = (row1[2] * row2[3] * row3[1] - row1[3] * row2[2] * row3[1] + row1[3] * row2[1] * row3[2] - row1[1] * row2[3] * row3[2] - row1[2] * row2[1] * row3[3] + row1[1] * row2[2] * row3[3]) * r;
    m.row0[1] = (row0[3] * row2[2] * row3[1] - row0[2] * row2[3] * row3[1] - row0[3] * row2[1] * row3[2] + row0[1] * row2[3] * row3[2] + row0[2] * row2[1] * row3[3] - row0[1] * row2[2] * row3[3]) * r;
    m.row0[2] = (row0[2] * row1[3] * row3[1] - row0[3] * row1[2] * row3[1] + row0[3] * row1[1] * row3[2] - row0[1] * row1[3] * row3[2] - row0[2] * row1[1] * row3[3] + row0[1] * row1[2] * row3[3]) * r;
    m.row0[3] = (row0[3] * row1[2] * row2[1] - row0[2] * row1[3] * row2[1] - row0[3] * row1[1] * row2[2] + row0[1] * row1[3] * row2[2] + row0[2] * row1[1] * row2[3] - row0[1] * row1[2] * row2[3]) * r;
    m.row1[0] = (row1[3] * row2[2] * row3[0] - row1[2] * row2[3] * row3[0] - row1[3] * row2[0] * row3[2] + row1[0] * row2[3] * row3[2] + row1[2] * row2[0] * row3[3] - row1[0] * row2[2] * row3[3]) * r;
    m.row1[1] = (row0[2] * row2[3] * row3[0] - row0[3] * row2[2] * row3[0] + row0[3] * row2[0] * row3[2] - row0[0] * row2[3] * row3[2] - row0[2] * row2[0] * row3[3] + row0[0] * row2[2] * row3[3]) * r;
    m.row1[2] = (row0[3] * row1[2] * row3[0] - row0[2] * row1[3] * row3[0] - row0[3] * row1[0] * row3[2] + row0[0] * row1[3] * row3[2] + row0[2] * row1[0] * row3[3] - row0[0] * row1[2] * row3[3]) * r;
    m.row1[3] = (row0[2] * row1[3] * row2[0] - row0[3] * row1[2] * row2[0] + row0[3] * row1[0] * row2[2] - row0[0] * row1[3] * row2[2] - row0[2] * row1[0] * row2[3] + row0[0] * row1[2] * row2[3]) * r;
    m.row2[0] = (row1[1] * row2[3] * row3[0] - row1[3] * row2[1] * row3[0] + row1[3] * row2[0] * row3[1] - row1[0] * row2[3] * row3[1] - row1[1] * row2[0] * row3[3] + row1[0] * row2[1] * row3[3]) * r;
    m.row2[1] = (row0[3] * row2[1] * row3[0] - row0[1] * row2[3] * row3[0] - row0[3] * row2[0] * row3[1] + row0[0] * row2[3] * row3[1] + row0[1] * row2[0] * row3[3] - row0[0] * row2[1] * row3[3]) * r;
    m.row2[2] = (row0[1] * row1[3] * row3[0] - row0[3] * row1[1] * row3[0] + row0[3] * row1[0] * row3[1] - row0[0] * row1[3] * row3[1] - row0[1] * row1[0] * row3[3] + row0[0] * row1[1] * row3[3]) * r;
    m.row2[3] = (row0[3] * row1[1] * row2[0] - row0[1] * row1[3] * row2[0] - row0[3] * row1[0] * row2[1] + row0[0] * row1[3] * row2[1] + row0[1] * row1[0] * row2[3] - row0[0] * row1[1] * row2[3]) * r;
    m.row3[0] = (row1[2] * row2[1] * row3[0] - row1[1] * row2[2] * row3[0] - row1[2] * row2[0] * row3[1] + row1[0] * row2[2] * row3[1] + row1[1] * row2[0] * row3[2] - row1[0] * row2[1] * row3[2]) * r;
    m.row3[1] = (row0[1] * row2[2] * row3[0] - row0[2] * row2[1] * row3[0] + row0[2] * row2[0] * row3[1] - row0[0] * row2[2] * row3[1] - row0[1] * row2[0] * row3[2] + row0[0] * row2[1] * row3[2]) * r;
    m.row3[2] = (row0[2] * row1[1] * row3[0] - row0[1] * row1[2] * row3[0] - row0[2] * row1[0] * row3[1] + row0[0] * row1[2] * row3[1] + row0[1] * row1[0] * row3[2] - row0[0] * row1[1] * row3[2]) * r;
    m.row3[3] = (row0[1] * row1[2] * row2[0] - row0[2] * row1[1] * row2[0] + row0[2] * row1[0] * row2[1] - row0[0] * row1[2] * row2[1] - row0[1] * row1[0] * row2[2] + row0[0] * row1[1] * row2[2]) * r;
    return m;
};

Triangle::Triangle (const Vec3 &v1, const Vec3 &v2, const Vec3 &v3) : v1 (v1), v2 (v2), v3 (v3) {}

Vec3 Triangle::Normal () const {
    Vec3 e1 = v2 - v1;
    Vec3 e2 = v3 - v1;
    return e1.Cross (e2).Normalize ();
}

Matrix Triangle::Quadric () const {
    Vec3 n = Normal ();
    double x = v1.x, y = v1.y, z = v1.z;
    double a = n.x, b = n.y, c = n.z;
    double d = -a * x - b * y - c * z;
    return Matrix{
        a * a, a * b, a * c, a * d,
        a * b, b * b, b * c, b * d,
        a * c, b * c, c * c, c * d,
        a * d, b * d, c * d, d * d};
}

Face::Face (Vertex const *v1, Vertex const *v2, Vertex const *v3) : v1 (v1), v2 (v2), v3 (v3) {}

bool Face::Degenerate () const {
    return v1->v == v2->v ||
           v1->v == v3->v ||
           v2->v == v3->v;
}

Vec3 Face::Normal () const {
    Vec3 e1 = v2->v - v1->v;
    Vec3 e2 = v3->v - v1->v;
    return e1.Cross (e2).Normalize ();
}

bool Face::operator< (const Face &other) const {
    if (v1->v != other.v1->v)
        return v1->v < other.v1->v;

    if (v2->v != other.v2->v)
        return v2->v < other.v2->v;

    return v3->v < other.v3->v;
}

bool Vec3::operator!= (const Vec3 &other) const {
    return !(*this == other);
}