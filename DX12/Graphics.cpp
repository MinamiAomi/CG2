#include "Graphics.h"

#include "Utility.h"

Graphics* Graphics::GetInstance() {
    static Graphics instance;
    return &instance;
}
