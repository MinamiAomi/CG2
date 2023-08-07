#pragma once

#include <winerror.h>

#include <cassert>
#include <string>

#ifdef ASSERT
#undef ASSERT
#endif 

#define ASSERT(x) assert(x)
#define ASSERT_SUCCEEDED(hr) ASSERT(SUCCEEDED(hr))

namespace CG {

    // wstringに変換
    std::wstring ConvertString(const std::string& str);
    // wstringから変換
    std::string ConvertString(const std::wstring& str);

}