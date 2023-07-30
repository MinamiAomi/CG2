#pragma once

namespace CG {

    class EditerManager;

    class Editer {
    public:
        Editer(EditerManager& editerManager) : editerManager_(editerManager) {}
        virtual ~Editer() {}
        virtual void Show() = 0;

    protected:
        EditerManager& editerManager_;
    };

}