#pragma once

#include "Geometry.hpp"
#include <filesystem>

namespace fs = std::filesystem;
namespace STL {
// https://en.wikipedia.org/wiki/STL_(file_format)#
struct Header {
    uint8_t _[80];
    uint32_t Count;
};

struct Triangle {
    float N[3], v1[3], v2[3], v3[3];
    uint16_t _; // attribute byte count
};

static constexpr size_t HEADER_SIZE = sizeof (Header);
static constexpr size_t TRIANGLE_SIZE = 12 * sizeof (float) + sizeof (uint16_t);
static_assert (HEADER_SIZE == 84, "Invalid STL header size");
static_assert (TRIANGLE_SIZE == 50, "Invalid STL triangle size");

std::vector<double> parseFloats (const std::vector<std::string> &items);

Mesh LoadBinary (fs::path const &path);
void SaveBinary (fs::path const &path, Mesh const &mesh);
Mesh LoadASCII (fs::path const &path);
}