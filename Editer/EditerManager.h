#pragma once

#include <memory>
#include <vector>

#include <unordered_map>

namespace CG {

    class Editer;

    class EditerManager {
    public:
        enum EditerType {
            Hierarchy,
            Inspector,
            Scene,
            Game,
            Resource,
            Console,

            EditerTypeCount
        };

        EditerManager();
        void ShowEditers() {}

        Editer& GetEditer(EditerType type) const { return *editers_[type]; }

    private:
        std::vector<std::unique_ptr<Editer>> editers_;
    };

}