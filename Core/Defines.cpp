#include "Defines.h"

#include <string>

void AssertIfFailed(HRESULT hr, const char* str, const char* file, const char* line) {
    if (FAILED(hr)) {
        std::string msg(file);
        msg += " @ ";
        msg += line;
        msg += "\n";
        msg += str;
        MessageBoxA(nullptr, msg.c_str(), "HRESUT failed", S_OK);
        ASSERT(false);
    }
}