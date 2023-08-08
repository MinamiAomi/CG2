#pragma once

#include <string>

namespace Utility {

    // wstringに変換
    std::wstring ConvertString(const std::string& str);
    // wstringから変換
    std::string ConvertString(const std::wstring& str);

    template<typename T>
    inline T AlignUpWithMak(T value, size_t mask) {
        return (T)((size_t(value + mask) & ~mask));
    }

    template<typename T>
    inline T AlignDownWithMak(T value, size_t mask) {
        return (T)((size_t(value) & ~mask));
    }

    template<typename T>
    inline T AlignUp(T value, size_t alignment) {
        return AlignUpWithMak(value, alignment - 1);
    }

    template<typename T>
    inline T AlignDown(T value, size_t alignment) {
        return AlignDownWithMak(value, alignment - 1);
    }

    template<typename T>
    inline bool IsAligned(T value, size_t alignment) {
        return (size_t(value) & (alignment - 1)) == 0;
    }

}