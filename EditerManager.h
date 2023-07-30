#pragma once

#include <vector>
#include <memory>

namespace CG {

    class Editer;

    class EditerManager {
    public:

    private:
        std::vector<std::unique_ptr<Editer>> editers_;
    };

}