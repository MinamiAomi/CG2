#pragma once

#include <string>

namespace CG {

    // wstringに変換
    std::wstring ConvertString(const std::string& str);
    // wstringから変換
    std::string ConvertString(const std::wstring& str);

}