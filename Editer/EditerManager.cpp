#include "EditerManager.h"

#include "HierarchyView.h"
#include "InspectorView.h"
#include "SceneView.h"
#include "GameView.h"
#include "ResourceView.h"
#include "ConsoleView.h"

namespace CG {

    EditerManager::EditerManager() {
        editers_[Hierarchy] = std::make_unique<HierarchyView>(*this);
        editers_[Inspector] = std::make_unique<InspectorView>(*this);
        editers_[Scene] = std::make_unique<SceneView>(*this);
        editers_[Game] = std::make_unique<GameView>(*this);
        editers_[Resource] = std::make_unique<ResourceView>(*this);
        editers_[Console] = std::make_unique<ConsoleView>(*this);
    }

}