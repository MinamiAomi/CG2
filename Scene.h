#pragma once

#include <memory>
#include <vector>

namespace CG {

    class GameObject;

    class Scene {
    public:
        Scene();
        std::shared_ptr<GameObject> AddGameObject();

        const std::shared_ptr<GameObject>& GetRoot() const { return root_; }
        const std::vector<std::shared_ptr<GameObject>>& GetGameObjects() const { return gameObjects_; }

    private:
        std::shared_ptr<GameObject> root_;
        std::vector<std::shared_ptr<GameObject>> gameObjects_;
    };

}