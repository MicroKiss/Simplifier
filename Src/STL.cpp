#include "STL.hpp"
#include <cstring>
#include <fstream>
#include <iosfwd>
#include <sstream>

std::vector<double> STL::parseFloats (const std::vector<std::string> &items) {
    std::vector<double> result;
    result.reserve (items.size ());

    for (const auto &item : items) {
        std::istringstream iss (item);
        double f;
        iss >> f;
        result.push_back (f);
    }

    return result;
}

Mesh STL::LoadBinary (fs::path const &path) {
    std::ifstream file (path, std::ios::binary);
    if (!file.is_open ()) {
        throw std::runtime_error ("Cannot open file " + path.string ());
    }

    Header header;
    file.read (reinterpret_cast<char *> (&header), HEADER_SIZE);
    if (!file) {
        throw std::runtime_error ("Error reading STL header");
    }

    Mesh mesh;
    mesh.reserve (header.Count);

    for (size_t i = 0; i < header.Count; ++i) {
        Triangle d;
        file.read (reinterpret_cast<char *> (&d), TRIANGLE_SIZE);
        if (!file) {
            throw std::runtime_error ("Error reading STL triangle");
        }
        mesh.emplace_back (
            Vec3 (d.v1[0], d.v1[1], d.v1[2]),
            Vec3 (d.v2[0], d.v2[1], d.v2[2]),
            Vec3 (d.v3[0], d.v3[1], d.v3[2]));
    }

    return mesh;
}

void STL::SaveBinary (fs::path const &path, Mesh const &mesh) {
    std::ofstream file (path, std::ios::binary);
    if (!file.is_open ()) {
        throw std::runtime_error ("Cannot create output file " + path.string ());
    }

    Header header{};
    header.Count = static_cast<uint32_t> (mesh.size ());

    const char *text = "Saved by Simplifier @MicroKiss";
    std::memcpy (header._, text, std::min (strlen (text), sizeof (header._)));
    file.write (reinterpret_cast<const char *> (&header), HEADER_SIZE);
    if (!file) {
        throw std::runtime_error ("Error writing STL header");
    }

    for (const ::Triangle &triangle : mesh) {
        Vec3 n = triangle.Normal ();
        Triangle d;
        d.N[0] = static_cast<float> (n.x);
        d.N[1] = static_cast<float> (n.y);
        d.N[2] = static_cast<float> (n.z);
        d.v1[0] = static_cast<float> (triangle.v1.x);
        d.v1[1] = static_cast<float> (triangle.v1.y);
        d.v1[2] = static_cast<float> (triangle.v1.z);
        d.v2[0] = static_cast<float> (triangle.v2.x);
        d.v2[1] = static_cast<float> (triangle.v2.y);
        d.v2[2] = static_cast<float> (triangle.v2.z);
        d.v3[0] = static_cast<float> (triangle.v3.x);
        d.v3[1] = static_cast<float> (triangle.v3.y);
        d.v3[2] = static_cast<float> (triangle.v3.z);
        file.write (reinterpret_cast<const char *> (&d), TRIANGLE_SIZE);
        if (!file) {
            throw std::runtime_error ("Error writing STL triangle");
        }
    }
}

Mesh STL::LoadASCII (fs::path const &path) {
    std::ifstream file (path);
    if (!file.is_open ()) {
        throw std::runtime_error ("Cannot open file " + path.string ());
    }

    std::vector<Vec3> vertexes;
    std::string line;
    while (std::getline (file, line)) {
        std::istringstream iss (line);
        std::vector<std::string> fields;
        std::string field;
        while (iss >> field) {
            fields.push_back (field);
        }
        if (fields.size () == 4 && fields[0] == "vertex") {
            std::vector<double> f = parseFloats ({fields.begin () + 1, fields.end ()});
            Vec3 v (f[0], f[1], f[2]);
            vertexes.push_back (v);
        }
    }

    Mesh mesh;
    mesh.reserve (vertexes.size ());
    for (size_t i = 0; i < vertexes.size (); i += 3) {
        mesh.emplace_back (
            vertexes[i + 0],
            vertexes[i + 1],
            vertexes[i + 2]);
    }

    return mesh;
}
