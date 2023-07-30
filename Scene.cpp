#include "Scene.h"

#include "GameObject.h"

namespace CG {

    

    Scene::Scene() {
        root_ = std::make_shared<GameObject>();
    }

    std::shared_ptr<GameObject> Scene::AddGameObject() {
        std::shared_ptr<GameObject> gameObject = std::make_shared<GameObject>();
        gameObject->SetParent(root_);
        gameObjects_.emplace_back(gameObject);
        return gameObject;
    }

}