#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>

#include "Math/MathUtils.h"

struct ObjectInfo {
    std::string objectName;

    std::vector<Vector3> positions;
    std::vector<Vector3> normals;
    std::vector<Vector2> texcoords;
    std::vector<uint32_t[3]> surfaceIndex;
};