#pragma once
#include "Editer.h"

namespace CG {

    class SceneView : public Editer {
    public:
        // 基底クラスのコンストラクタを定義
        using Editer::Editer;

        void Show() override {}
    };

}