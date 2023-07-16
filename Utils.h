#pragma once

#include <cstdint>

namespace CG {

    struct Sizef {
        float width;
        float height;

        Sizef Half() const { return { width * 0.5f, height * 0.5f }; }
    };

    struct Sizeui {
        uint32_t width;
        uint32_t height;

        Sizeui Half() const { return { width / 2, height / 2 }; }
    };

}